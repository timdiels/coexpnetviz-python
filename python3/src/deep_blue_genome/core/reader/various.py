import pandas
import csv
from plumbum import local
from deep_blue_genome.core.expression_matrix import ExpressionMatrix

# TODO validation should be part of reading. We at least want to save a log of
# warnings. TODO input validation error handling + setting stuff as warning or
# error. Different users care about different things, in different use cases. By
# default there should be a strict config and a more lenient config. Sadly, the
# lenient config should be picked as default. In either case, errors and warnings
# will be logged. Though keep in mind that in the future one might even want to limit logging.
'''
All reading/input things.

Input validation error handling can be configured to either:

- show and/or log an error and attempt to recover
- throw an exception
'''

# TODO test promises in robustness for the various file formats

# TODO in the future this may be of interest https://pypi.python.org/pypi/python-string-utils/0.3.0  We could donate our own things to that library eventually...

def sanitise_plain_text_file(file):
    '''
    Sanitise plain text file.
    
    - Remove null characters
    - Fix newlines, drop empty lines
    - Replace multiple tabs by single tab.
    
    Parameters
    ----------
    file : str-like
        string to sanitise
    '''
    sed = local['sed']
    sed('-ir', '-e', u's/[\\x0]//g', '-e', 's/[\\r\\n]+/\\n/g', '-e', 's/(\t)+/\t/g', file)

# TODO consider
def read_expression_matrix_file(path):
    '''
    Read expression matrix file.
    
    This file has 1 header line and 0 or more rows. The first column is the gene
    (or gene variant) name. The other columns contain the expression values of
    each condition.
    
    Lines are separated by one or more newline characters (any amount of \r or
    \n). Empty lines are ignored. Columns are separated by any whitespace other
    than newline characters. Empty fields are ignored.
    
    Parameters
    ----------
    path : str
        path to expression matrix file to read
    
    Returns
    -------
    ExpressionMatrix
        The expression matrix
    '''
    mat = pandas.read_table(path, index_col=0, header=0, engine='python')
    mat.index = mat.index.str.lower()
    return ExpressionMatrix(mat, name=local.path(path).name)

def read_whitespace_separated_2d_array_file(path):
    '''
    Read whitespace separated 2d array file.
    
    Unlike a matrix, rows may differ in field length.
    
    Lines act as rows, the first dimension. Words in a line act as the second
    dimension. So `array[1][2]` is the third word on the second row.
    
    Lines are separated by one or more newline characters (any amount of \r or
    \n). Empty lines are ignored. Words are separated by any whitespace other
    than newline characters. Empty words are ignored.
    
    Parameters
    ----------
    path : str
        path to 2d array file to read
    
    Returns
    -------
    list of list of str
        2d array
    '''
    with open(path, encoding="utf-8") as f:
        lines = f.read().split("[\r\n]+")
        lines = [line.split() for line in lines]
        return lines

# TODO this docstring may be too hard on users 
def read_gene_families_file(path):
    '''
    Read a gene families file.
    
    Expected format: csv format with tabs as separator (instead of ',').
    
    Each row is a gene family: `family_name gene1 gene2 ...`. (A gene may occur
    in multiple gene families).
    
    Parameters
    ----------
    path : str
        path to gene families file to read
    
    Returns
    -------
    { family_name -> {gene : str} }
        Gene families
    '''
    sanitise_plain_text_file(path)
    with open(path, 'r') as f:
        reader = csv.reader(f, delimiter="\t")
        return {row[0].lower() : set(map(str.lower, row[1:])) for row in reader}

def read_baits_file(path):
    '''
    Read a file of whitespace separated bait gene names.
    
    Parameters
    ----------
    path : str
        path to file to read
    
    Returns
    -------
    list of str
        List of genes
    '''
    with open(path, encoding='utf-8') as f:
        return f.read().split()



