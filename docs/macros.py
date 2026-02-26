import re

def camel_to_snake(name):
    # Insert underscore before capital letters (except first), lowercase everything
    s1 = re.sub('(.)([A-Z][a-z]+)', r'\1_\2', name)
    s2 = re.sub('([a-z0-9])([A-Z])', r'\1_\2', s1)
    return s2.lower()

def define_env(env):
    """
    This function is called by mkdocs-macros-plugin.
    You can define variables and functions here.
    """

    # Load variables from an external file (YAML, JSON, or Python dict)

    @env.macro
    def cls(name):
        return f"[`{name}`](/Renderer/class{name})"