import mmap
import mmap_hl_spinlock
import multiprocessing
import os

from time import sleep

def main():
    region = mmap.mmap(-1, mmap.PAGESIZE, flags=mmap.MAP_SHARED)
    # Testing that lock doesn't block
    mmap_hl_spinlock.mmap_hllock(region)
    mmap_hl_spinlock.mmap_hlunlock(region)
    res = os.fork()
    if res == 0:
        mmap_hl_spinlock.mmap_hllock(region)
        print(1)
        sleep(1)
        print(2)
        mmap_hl_spinlock.mmap_hlunlock(region)
        return
    mmap_hl_spinlock.mmap_hllock(region)
    print(3)
    sleep(1)
    print(4)
    mmap_hl_spinlock.mmap_hlunlock(region)
    os.wait()


if __name__ == '__main__':
    main()