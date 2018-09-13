# copied over unmodified from
# https://github.com/devpi/devpi/blob/master/common/devpi_common/types.py


def cached_property(f):
    """returns a cached property that is calculated by function f"""
    def get(self):
        try:
            return self._property_cache[f]
        except AttributeError:
            self._property_cache = {}
        except KeyError:
            pass
        x = self._property_cache[f] = f(self)
        return x

    def set(self, val):
        propcache = self.__dict__.setdefault("_property_cache", {})
        propcache[f] = val
    return property(get, set)