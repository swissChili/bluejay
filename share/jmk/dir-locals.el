((nil . ((eval
		  . (setq-local flycheck-clang-include-path
						(jmk-includes-for (expand-file-name buffer-file-name))
						flycheck-clang-args
						(jmk-other-flags-for (expand-file-name buffer-file-name)))))))
