import sys
import os
sys.path.append(os.path.dirname('./build/lib.linux-x86_64-3.8/mmap_hl_spinlock.cpython-38-x86_64-linux-gnu.so'))
import mmap
import mmap_hl_spinlock
import multiprocessing
import os

from time import sleep

def main():
    region_fd = open("/tmp/test-lock", "r+")
    region = mmap.mmap(region_fd.fileno(), mmap.PAGESIZE, flags=mmap.MAP_SHARED)
    # Testing that lock doesn't block
    mmap_hl_spinlock.lock()
    mmap_hl_spinlock.mmap_hllock(region)
    mmap_hl_spinlock.unlock()


if __name__ == '__main__':
    main()