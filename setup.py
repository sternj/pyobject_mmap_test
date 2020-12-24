from distutils.core import setup, Extension

mmap_hl_spinlock = Extension('mmap_hl_spinlock',
                    include_dirs=['.', 'heap-layers'],
                    sources = ['mmap_hl_spinlock.cpp'])

setup (name = 'MmapHlSpinlock',
       version = '1.0',
       description = 'This is a demo package',
       ext_modules = [mmap_hl_spinlock])