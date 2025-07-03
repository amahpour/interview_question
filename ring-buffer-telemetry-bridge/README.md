## Ring-Buffer Telemetry Bridge
*Single-producer C ring buffer + Python consumer harness*

---

### Background

In a virtual-ECU stack, real firmware often pushes bytes into a lock-free queue while a host-side Python test harness streams the data for analysis.
This mini-exercise recreates that pattern on a smaller scale.

You will **write code in two files**:

| File              | Language     | What you must do                                                                                                                                                                                               |
| ----------------- | ------------ | -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| `ringbuf.c`       | **C**        | Implement a thread-safe single-producer / single-consumer (SPSC) ring buffer using **only POSIX primitives** (`pthread_mutex`, `pthread_spinlock`, or similar).                                                |
| `check_stream.py` | **Python 3** | Launch the compiled C program, read its stdout in real time, and verify that the byte stream is strictly incrementing *modulo 256*. Print `PASS` (and exit 0) on success; otherwise print `FAIL` and exit ≠ 0. |

The autograder runs a shell script that (1) compiles the C code, (2) executes your Python harness, and (3) awards full credit only if the script prints **`PASS`** and exits with status 0.

---

### Starter files

```
├── ringbuf.h          ← Header (read-only)
├── ringbuf.c          ← **YOU implement here**
├── main.c             ← Producer + consumer demo (read-only)
├── check_stream.py    ← **YOU implement here**
└── run.sh             ← Grader script (read-only)
```

#### Provided code snippets

`ringbuf.h`

```c
#ifndef RINGBUF_H
#define RINGBUF_H
#include <stdbool.h>
#include <stddef.h>

void  rb_init(unsigned char *buffer, size_t capacity);
bool  rb_push(unsigned char byte);         /* false ⇒ overflow */
bool  rb_pop (unsigned char *out_byte);    /* false ⇒ underflow */

#endif
```

`main.c` (excerpt)

```c
#define CAPACITY 64
static unsigned char storage[CAPACITY];

void *producer(void *_) {
    unsigned char x = 0;
    while (1) {
        rb_push(x++);
        usleep(1000);          /* 1 kHz */
    }
}

int main(void) {
    rb_init(storage, CAPACITY);

    pthread_t th;
    pthread_create(&th, NULL, producer, NULL);

    unsigned char v;
    while (1) {
        if (rb_pop(&v)) {
            printf("%u\n", v);
            fflush(stdout);    /* line-flush for Python reader */
        }
    }
}
```

`run.sh`

```bash
#!/usr/bin/env bash
set -e
gcc -std=c11 -pthread ringbuf.c main.c -o app
python3 check_stream.py | grep -q '^PASS$'
```

---

### Your tasks

#### 1. `ringbuf.c`

Implement:

* `rb_init` – store buffer pointer, capacity, and reset indices.
* `rb_push` – enqueue one byte; return **false** if the buffer is full.
* `rb_pop`  – dequeue one byte into `*out_byte`; return **false** if empty.

Requirements

* Safe for **one producer thread** (`producer` in `main.c`) and **one consumer thread** (the main loop).
* Must not busy-wait indefinitely.
* Use only standard C11 + POSIX (`pthread_*`, `stddef.h`, `stdint.h`).

#### 2. `check_stream.py`

* Spawn `./app` with `subprocess.Popen`.
* Read each stdout line as an integer byte value.
* Ensure the sequence is `0,1,2,…255,0,1,2,…` without gaps or reversals.
* After validating **1 000** bytes, print exactly `PASS` and exit 0.
* On any error (timeout, invalid integer, out-of-sequence, premature EOF) print `FAIL` and exit with non-zero status.

Hint: use `proc.terminate()` to stop the C program after you’ve validated enough data.

---

### Building & running locally

```bash
chmod +x run.sh check_stream.py
./run.sh          # should print PASS and exit 0 when your code is correct
```

The autograder executes the same command with a 5-second timeout.

---

### Scoring rubric

| Criterion                          | Weight |
| ---------------------------------- | ------ |
| Correctness (prints PASS, exit 0)  | 70 %   |
| Concurrency safety (no data races) | 15 %   |
| Code clarity & idiomatic style     | 10 %   |
| Efficiency / no busy-waiting       | 5 %    |

Good luck—and remember to keep both implementations concise.
