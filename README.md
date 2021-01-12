# HL::Spinlock mmap test

To make this not block when it should, run
```
python3 setup.py build && python3 mmap_hl_spinlock_test.py && rm /tmp/test-*
```