import os

env=os.environ

def complete_array(a,s,form="%.18f,\n"):
    for x in a:
        s+=form%(x,)
    s+="""};
    """
    return s

def get_env(name,default=None,conv=lambda x: x):
    result = default
    if name in env:
        result = conv(env[name])
    if result is None:
        raise Exception('Specify %s' % (name,))
    #past_get_env_args[name] = result
    return result
