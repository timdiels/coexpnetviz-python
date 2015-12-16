import plumbum as pb
from deep_blue_genome.main import main, load_config
from deep_blue_genome.core.util import flatten_deep
from copy import deepcopy

cache_dir = pb.local.path('test/cache')  # cache across runs
data_dir = pb.local.path('test/data')

_config, _main_config = load_config()  # TODO maybe we could have a programatically put together conf instead of one read from the system, which is less isolated

def get_config(section, tmp_dir):
    config = deepcopy(_config[section])
    config.update(
#         main_config=deepcopy(_main_config),  #TODO can't deepcopy
        tmp_dir=tmp_dir,
        database_name=config['database_name'] + '_test',
    )
    return config
    
def get_data_file(relative_path):
    return data_dir / relative_path

def assert_same_file(actual, expected):
    assert actual.name == expected.name
    assert actual.read() == expected.read()
    
class CLITester(object):

    def __init__(self, args):
        '''
        args : list-like of any
            Args to always pass to the CLI
        '''
        self._args = args
    
    @property
    def tmpdir(self, value):
        return self._tmp_dir    
    
    @tmpdir.setter
    def tmpdir(self, value):
        self._tmp_dir = value

    def copy_data(self, item, prefix=''):    
        '''
        Copy data to tmpdir, return basename of passed item or the basenames of its content.
        
        Parameters
        ----------
        item : dict, list or str or plumbum.Path
            If dict, copy_data is mapped to the dict values and the mapped dict
            is returned. Analog for a list. If str of relative path, it is
            prefixed with `test data directory / prefix` to make an absolute
            path. If str is an absolute path or plumbum.Path, it is taken as is
            and its file name is returned after the copy.
        prefix : str
            Relative paths are prefixed with `test data directory / prefix`
        '''
        if isinstance(item, dict):
            return {k : self.copy_data(v, prefix) for k,v in item.items()}
        elif isinstance(item, list):
            return [self.copy_data(v, prefix) for v in item]
        else:    
            if isinstance(item, str):
                if item.startswith('/'):
                    item = pb.local.path(item)
                else:
                    item = data_dir / prefix / item
            item.copy(self._tmp_dir)
            return item.name
        
    def run(self, *args, **kwargs):
        args = self._args + list(args) + flatten_deep([['--' + k.replace('_', '-'), v] for k,v in kwargs])
        args = list(map(str, args))
        with pb.local.cwd(self._tmp_dir):
            try:
                main(args)
            except SystemExit as ex:
                if ex.code != 0:
                    raise
            
            