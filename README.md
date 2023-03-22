[testing-image]: https://github.com/pyg-team/pyg-lib/actions/workflows/testing.yml/badge.svg
[testing-url]: https://github.com/pyg-team/pyg-lib/actions/workflows/testing.yml
[docs-image]: https://readthedocs.org/projects/pyg-lib/badge/?version=latest
[docs-url]: https://pyg-lib.readthedocs.io/en/latest/?badge=latest
[coverage-image]: https://codecov.io/gh/pyg-team/pyg-lib/branch/master/graph/badge.svg
[coverage-url]: https://codecov.io/github/pyg-team/pyg-lib?branch=master

# pyg-lib

[![Testing Status][testing-image]][testing-url]
[![Docs Status][docs-image]][docs-url]
[![Code Coverage][coverage-image]][coverage-url]

* [Installation](#installation)

## Installation

```shell
CC="fcc -Nclang -Knolargepage" CXX="FCC -Nclang -Knolargepage" LC_ALL=C pip install git+https://github.com/ficstamas/pyg-lib-fugaku.git
```

You can also set:
- `FVENV_PATH`: Absolute path to the directory which contains your virtual environment (Default: `/local`) 
- `FVENV_NAME`: Name of your environment (Default: `venv`)
- `FVENV_PYTHON`: Python version (Default: `python3.9`)

These variables are required to locate `torch` in the following manner:

```
${FVENV_PATH}/${FVENV_NAME}/lib/${FVENV_PYTHON}/site-packages/torch/share/cmake/Torch/
```