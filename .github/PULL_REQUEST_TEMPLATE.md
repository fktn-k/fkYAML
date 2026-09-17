[Describe your pull request here. Please read the description below and make sure you follow the checklist.]  

---

## Pull Request Checklist

Read the [CONTRIBUTING.md](https://github.com/fktn-k/fkYAML/blob/develop/.github/CONTRIBUTING.md) file for detailed information.  

- [ ] Changes are described in the pull request or in a referenced [issue](https://github.com/fktn-k/fkYAML/issues).
- [ ] The test suite compiles and runs without any error.
- [ ] [The code coverage](https://coveralls.io/github/fktn-k/fkYAML) on your branch is 100%.
- [ ] The documentation is updated if you added/changed a feature.

## Please don't

- The C++11 support varies between different **compilers** and versions. Please note the [list of supported compilers](https://github.com/fktn-k/fkYAML/blob/develop/README.md#supported-compilers). Some compilers like GCC 4.7 (and earlier), Clang 3.3 (and earlier), or Microsoft Visual Studio 13.0 and earlier are known not to work due to missing or incomplete C++11 support. Please refrain from proposing changes that work around these compiler's limitations with `#ifdef`s or other means.
- Please refrain from proposing changes that would **break [YAML](https://yaml.org/) specifications**. If you propose a conformant extension of YAML to be supported by the library, please motivate this extension.
- Please do not open pull requests that address **multiple issues**.
