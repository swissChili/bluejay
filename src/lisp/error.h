#pragma once

#include <stdbool.h>

// Error handling code

struct eloc
{
	int line;
	char *file;
};

enum error_code
{
	EOK = 0,
	/// Expected something but didn't get it. if this is in a
	/// safe_state we should probably just re-try.
	EEXPECTED,
	/// An invalid token was present in the input
	EINVALID,
	/// A structure was malformed
	EMALFORMED,
	/// The arguments provided were invalid
	EARGS,
	/// An external resource (say, a file) was not found
	ENOTFOUND,
	/// This is unimplemented
	EUNIMPL,
};

struct error
{
	enum error_code code;
	// Is any state safe? I.e. can we continue or must we panic?
	bool safe_state;
	struct eloc loc;
	char *message;
};

#define E_INIT()                                                               \
	struct error __error;                                                      \
	__error.code = EOK;                                                        \
	__error.loc.line = 0;                                                      \
	__error.safe_state = false;                                                \
	__error.message = NULL;                                                    \
	__error.loc.file = NULL;
#define NEARVAL(val)                                                           \
	__error.loc.line = cons_line(val);                                         \
	__error.loc.file = cons_file(val)
#define NEARIS(is) (is)->getpos((is), &__error.loc.line, &__error.loc.file)
#define _TRY(expr, m, c)                                                       \
	{                                                                          \
		struct error __sub = (expr);                                           \
		if (__sub.code)                                                        \
		{                                                                      \
			if (!__sub.loc.file || !__sub.loc.line)                            \
				__sub.loc.file = __error.loc.file,                             \
				__sub.loc.line = __error.loc.line;                             \
			if (c)                                                             \
				__sub.code = c;                                                \
			if (m)                                                             \
				__sub.message = m;                                             \
			return __sub;                                                      \
		}                                                                      \
	}
#define TRY(expr) _TRY(expr, NULL, 0)
#define TRY_ELSE(expr, c, ...) _TRY(expr, ehsprintf(__VA_ARGS__), c)
#define OKAY() return __error
#define THROW(_c, ...)                                                         \
	{                                                                          \
		__error.code = (_c);                                                   \
		__error.message = ehsprintf(__VA_ARGS__);                              \
		return __error;                                                        \
	}
#define THROWSAFE(_c)                                                          \
	{                                                                          \
		__error.code = (_c);                                                   \
		__error.safe_state = true;                                             \
		return __error;                                                        \
	}

#define IS_OKAY(e) ((e).code == EOK)
#define OKAY_IF(val)                                                           \
	{                                                                          \
		struct error __sub = (val);                                            \
		if (IS_OKAY(__sub))                                                    \
			OKAY();                                                            \
		if (!__sub.safe_state)                                                 \
			TRY(__sub)                                                         \
	}

#define WARN_UNUSED __attribute__((warn_unused_result))

// error heap string print formatted
// returns a heap-allocated string.
char *ehsprintf(const char *msg, ...);

void ereport(struct error err);
