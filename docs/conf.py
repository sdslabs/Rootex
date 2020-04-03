# Configuration file for the Sphinx documentation builder.
#
# This file only contains a selection of the most common options. For a full
# list see the documentation:
# https://www.sphinx-doc.org/en/master/usage/configuration.html

# -- Path setup --------------------------------------------------------------

# If extensions (or modules to document with autodoc) are in another directory,
# add these directories to sys.path here. If the directory is relative to the
# documentation root, use os.path.abspath to make it absolute, like shown here.
#
import os
import sys
sys.path.insert(0, os.path.abspath('.'))


# -- Project information -----------------------------------------------------

project = 'Rootex'
copyright = '2020, SDSLabs'
author = 'SDSLabs'
master_doc = 'index'

# -- Source information ------------------------------------------------------

source_parsers = {
}

source_suffix = ['.rst', '.md']

# -- General configuration ---------------------------------------------------

# Add any Sphinx extension module names here, as strings. They can be
# extensions coming with Sphinx (named 'sphinx.ext.*') or your custom
# ones.
extensions = [
    'recommonmark',
    'breathe',
    'exhale'
]

breathe_projects = {
    "Rootex": "build/xml"
}
breathe_default_project = "Rootex"

exhale_args = {
    # These arguments are required
    "verboseBuild":          True,
    "containmentFolder":     "api",
    "rootFileName":          "rootex.rst",
    "rootFileTitle":         "Rootex",
    "doxygenStripFromPath":  "../",
    # Suggested optional arguments
    "createTreeView":        True,
    # TIP: if using the sphinx-bootstrap-theme, you need
    # "treeViewIsBootstrap": True,
    "exhaleExecutesDoxygen": True,
    "exhaleUseDoxyfile":     True
}

primary_domain = 'cpp'
highlight_language = 'cpp'

# Add any paths that contain templates here, relative to this directory.
templates_path = ['_templates']

html_static_path = ['_static']

# List of patterns, relative to source directory, that match files and
# directories to ignore when looking for source files.
# This pattern also affects html_static_path and html_extra_path.
exclude_patterns = []

# -- Options for HTML output -------------------------------------------------

# The theme to use for HTML and HTML Help pages.  See the documentation for
# a list of builtin themes.
#
html_theme = 'sphinx_rtd_theme'

# def setup(app):
#     app.add_stylesheet('css/custom.css')
