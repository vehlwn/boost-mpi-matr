import conans

# https://docs.conan.io/en/latest/reference/build_helpers/meson.html
class BoostMpiMatrConan(conans.ConanFile):
    generators = "pkg_config"
    settings = "os", "compiler", "build_type", "arch"
    requires = ["boost/1.80.0", "eigen/3.4.0", "openmpi/4.1.0", "zlib/1.2.13"]
    default_options = {"boost:without_mpi": False}

    def build(self):
        meson = conans.Meson(self)
        meson.configure()
        meson.build()
