import subprocess, os

rtd = os.environ.get('READTHEDOCS', None) == 'True'

if rtd:
	print('Generating kernel doxygen')
	subprocess.call('doxygen kernel.doxyfile', shell=True)
	print('Generating lisp doxygen')
	subprocess.call('doxygen lisp.doxyfile', shell=True)

project = 'Bluejay'
copyright = '2021, swissChili'
author = 'swissChili'

extensions = [
	'breathe'
]

breathe_projects = {
	'Kernel': '_dox_kernel/xml',
	'Lisp': '_dox_lisp/xml'
}

breathe_default_project = 'Kernel'

templates_path = ['_templates']

exclude_patterns = ['_build', '_dox_kernel', '_dox_lisp', 'Thumbs.db', '.DS_Store']

html_theme = 'sphinx_rtd_theme'

html_static_path = ['_static']
