# Installation

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

# Faster Installation

Clone the repository to `/local/` and install it locally, otherwise `pip` clones everything into `~/pip-req-build-*` which is much slower.

```shell
git clone --recurse-submodules git@github.com:ficstamas/pyg-lib-fugaku.git /local/.tmp/pyg-lib
CC="fcc -Nclang -Knolargepage" CXX="FCC -Nclang -Knolargepage" LC_ALL=C pip install /local/.tmp/pyg-lib
```