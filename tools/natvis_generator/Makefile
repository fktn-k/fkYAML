install-venv:
	python3 -m venv venv
	venv/bin/pip install --upgrade pip
	venv/bin/pip install -r requirements.txt

uninstall-venv:
	rm -rf venv

generate: install-venv
	venv/bin/python3 ./natvis_generator.py
