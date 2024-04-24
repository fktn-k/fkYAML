# build the site
build: install-venv
	venv/bin/mkdocs build

# serve the site locally
serve: build
	venv/bin/mkdocs serve

# install a Python virtual environment
install-venv: requirements.txt
	python3 -m venv venv
	venv/bin/pip install --upgrade pip
	venv/bin/pip install -r requirements.txt

uninstall-venv:
	rm -rf venv
