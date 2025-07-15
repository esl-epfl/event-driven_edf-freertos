# FreeRTOS with Dynamic EDF Scheduling for Event-Driven Systems

This repository contains a modified version of the FreeRTOS kernel that implements a low-overhead, dynamic Earliest Deadline First (EDF) scheduler. It is designed to improve performance and resource utilization in real-time embedded systems, particularly those with event-driven workloads.

---
## ✨ Key Features

* ✅ **Optimal Uniprocessor Scheduling:** EDF is a proven optimal scheduling algorithm for uniprocessor systems. If a set of tasks can be scheduled by any algorithm, it can be scheduled by EDF.

* 🚀 **Reduces Deadline Misses:** In event-driven scenarios, this dynamic scheduler can reduce missed task deadlines by up to **60%** compared to a standard static-priority scheduler.

* 💡 **Low-Overhead Implementation:** The scheduler is implemented with minimal overhead by using a single, deadline-sorted ready list. This results in up to **34.7%** lower timing overhead and **74.7%** lower memory overhead compared to the native FreeRTOS implementation.

---

## 🚀 Getting Started

### Prerequisites

* An ARM development environment (e.g., GCC for Arm, Keil, IAR).
* An existing FreeRTOS-based project.

### 1. Integration

To use the EDF scheduler, replace the standard FreeRTOS source files in your project with the files from the `FreeRTOS/` directory of this repository.

### 2. Configuration

In your project's `FreeRTOSConfig.h` file, you must add the following definitions to enable and configure the dynamic scheduler:

```c
/* Set to 1 to enable dynamic EDF scheduling.
 * Set to 0 to use the standard static priority scheduler. */
#define configDYNAMIC_SCHEDULING          1

/* Set to 1 to include the vTaskDeadlineRelSet() API function. */
#define INCLUDE_vTaskDeadlineRelSet       1

/* We recommend using the new scheduler with preemption and time slicing enabled. */
#define configUSE_PREEMPTION              1
#define configUSE_TIME_SLICING            1
```
---
## ⚙️ API Reference

The core FreeRTOS API remains largely the same, with the following key additions and changes.

### `xTaskCreate()`

The `xTaskCreate()` function is extended to accept a relative deadline as its final parameter.

```c
BaseType_t xTaskCreate(
    TaskFunction_t       pvTaskCode,
    const char * const   pcName,
    configSTACK_DEPTH_TYPE usStackDepth,
    void * pvParameters,
    UBaseType_t          uxPriority,        /* Used only in static scheduling mode */
    TaskHandle_t * pxCreatedTask,
    TickType_t           xRelativeDeadline  /* New parameter for EDF! */
);
```

* `xRelativeDeadline`: Specifies the task's relative deadline in system ticks. This is the maximum time allowed from when the task becomes ready until it must complete its job.
* If `configDYNAMIC_SCHEDULING` is `1`, the `uxPriority` parameter is **ignored**.
* If `configDYNAMIC_SCHEDULING` is `0`, the `xRelativeDeadline` parameter is **ignored**, and the scheduler falls back to using `uxPriority`.

### `vTaskDeadlineRelSet()`

If `INCLUDE_vTaskDeadlineRelSet` is enabled, you can dynamically change a task's relative deadline at runtime. This is useful for tasks whose urgency or execution requirements change.

```c
void vTaskDeadlineRelSet(TaskHandle_t xTask, TickType_t xRelativeDeadline);
```

---

## 📊 Usage Example

- The `main.c` file in this repository provides a clear example demonstrating the benefit of the dynamic EDF scheduler over a static, Deadline Monotonic (DM) scheduler.

<!-- The example creates two periodic tasks with different deadlines. Task 1 has a short deadline, and Task 2 has a longer deadline. However, a scenario is created where Task 2's absolute deadline becomes more imminent.

### Example Code

```c
// Example task creation in main.c
// Task 1: Short relative deadline
xTaskCreate(vPeriodicTask, "T1", 1000, NULL, 2, NULL, 20); // 20-tick deadline

// Task 2: Longer relative deadline
xTaskCreate(vPeriodicTask, "T2", 1000, NULL, 1, NULL, 50); // 50-tick deadline
``` -->
<!-- ### Expected Behavior

#### With Dynamic EDF Scheduling (`configDYNAMIC_SCHEDULING = 1`)

The scheduler dynamically executes the task with the nearest **absolute deadline**. The output will show tasks with longer relative deadlines (like `T2`) running before tasks with shorter ones (`T1`) if their absolute deadline is more imminent, preventing misses.

```console
[Tick 10] T1 completes job. Next deadline: 30
[Tick 20] T2 completes job. Next deadline: 70
[Tick 30] T1 completes job. Next deadline: 50
[Tick 50] T1 completes job. Next deadline: 70
[Tick 70] T2 completes job. Next deadline: 120 <-- T2 runs because its absolute deadline (70) was nearer than T1's (also 70, but T2 was ready first or chosen by policy).
[Tick 70] T1 completes job. Next deadline: 90
... All deadlines are met.
```

#### With Static Priority Scheduling (`configDYNAMIC_SCHEDULING = 0`)

The static scheduler uses the fixed priorities assigned at creation (Deadline Monotonic, where shorter deadline = higher priority). `T1` (`uxPriority = 2`) will always preempt `T2` (`uxPriority = 1`). This can lead to deadline misses for `T2`, even if its deadline is imminent.

```console
[Tick 10] T1 completes job. Next deadline: 30
[Tick 20] T1 preempts T2.
[Tick 30] T1 completes job. Next deadline: 50
[Tick 50] T1 completes job. Next deadline: 70
[Tick 65] T2 finally runs, but has missed its deadline of 50.
... Deadline miss for T2!
```
--- -->
---

## 📜 Citation

If you use this work in your research, please cite the original paper. [Link to the paper on IEEE Xplore](https://ieeexplore.ieee.org/abstract/document/10321845):

```bibtex
@inproceedings{taji2023dynamic,
  title={Dynamic Scheduling for Event-Driven Embedded Industrial Applications},
  author={Taji, Hossein and Miranda, Jos{\'e} and Pe{\l}o{\'n}-Quir{\'o}s, Miguel and Balasi, Szabolcs and Atienza, David},
  booktitle={2023 IFIP/IEEE 31st International Conference on Very Large Scale Integration (VLSI-SoC)},
  pages={1--6},
  year={2023},
  organization={IEEE}
}
```
---

## ⚖️ License

This project is licensed under the MIT License. See the `LICENSE` file for details.