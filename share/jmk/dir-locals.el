((nil . ((eval
		  . (let ((path (expand-file-name (or buffer-file-name "."))))
			  (setq-local flycheck-clang-include-path
						(jmk-includes-for path)
						flycheck-clang-args
						(jmk-other-flags-for path)))))))
