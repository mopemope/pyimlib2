from setuptools import setup, find_packages, Extension
import os
import sys
import os.path

readme = ""

if "posix" not in os.name:
    print "Are you really running a posix compliant OS ?"
    print "Be posix compliant is mandatory"
    sys.exit(1)

library_dirs=['/usr/local/lib']
include_dirs=[]



setup(name='pyimlib2',
        version="0.1",
        description="",
        long_description=readme,
        keywords='',
        author='yutaka matsubara',
        author_email='yutaka.matsubara@gmail.com',
        test_suite = 'nose.collector',        
        packages= find_packages(),
        ext_modules = [
            Extension('pyimlib2',
            sources=['src/imlib2.c', 'src/image.c'],
            include_dirs=include_dirs,
            library_dirs=library_dirs,
            libraries=["Imlib2"],
            define_macros=[("DEVELOP",None)],
            )
        ]
)


