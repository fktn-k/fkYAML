#!/usr/bin/env python3

import json
import jinja2
import re
import sys

def check_version_format(ver):
    if not re.fullmatch(r'\d+\.\d+\.\d+', ver):
        raise ValueError('Invalid semantic version specified. ver=' + ver)
    return ver


if __name__ == '__main__':

    with open('params.json') as j:
        params= json.load(j)

    semver = check_version_format(params['version'])
    abi_ns = 'v' + semver.replace('.', '_')
    namespace = 'fkyaml::' + abi_ns

    environment = jinja2.Environment(loader=jinja2.FileSystemLoader(searchpath=sys.path[0]),
                                     autoescape=True,
                                     trim_blocks=True,
                                     lstrip_blocks=True,
                                     keep_trailing_newline=True)
    template = environment.get_template('fkYAML.natvis.j2')

    natvis = template.render(namespace=namespace)

    with open('../../fkYAML.natvis', 'w') as f:
        f.write(natvis)
