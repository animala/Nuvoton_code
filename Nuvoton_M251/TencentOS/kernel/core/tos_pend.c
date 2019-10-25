#include "tos.h"

__STATIC__ void pend_list_add(k_task_t *task, pend_obj_t *pend_obj)
{
    k_list_t *curr, *pend_list;
    k_task_t *iter;

    pend_list = &pend_obj->list;
    /* keep priority in descending order, the boss(task with highest priority,
       numerically smallest) always comes first
    */
    TOS_LIST_FOR_EACH(curr, pend_list) {
        iter = TOS_LIST_ENTRY(curr, k_task_t, pend_list);
        if (task->prio < iter->prio) {
            break;
        }
    }
    tos_list_add_tail(&task->pend_list, curr);

    // remember me, you may use me someday
    task->pending_obj = pend_obj;
    task_state_set_pend(task);
}

__KERNEL__ k_prio_t pend_highest_prio_get(pend_obj_t *object)
{
    k_task_t *task;

    // we keep the task priority in descending order, so the first one is just fine.
    task = TOS_LIST_FIRST_ENTRY_OR_NULL(&object->list, k_task_t, pend_list);
    return task ? task->prio : K_TASK_PRIO_INVALID;
}

__KERNEL__ void pend_list_remove(k_task_t *task)
{
    tos_list_del(&task->pend_list);

    task->pending_obj = (pend_obj_t *)K_NULL;
    task_state_reset_pending(task);
}

__KERNEL__ void pend_object_init(pend_obj_t *object, pend_type_t type)
{
    object->type = type;
    tos_list_init(&object->list);
}

__KERNEL__ void pend_object_deinit(pend_obj_t *object)
{
    object->type = PEND_TYPE_NONE;
    tos_list_init(&object->list);
}

__KERNEL__ int pend_is_nopending(pend_obj_t *object)
{
    return tos_list_empty(&object->list);
}

__KERNEL__ void pend_list_adjust(k_task_t *task)
{
    // we may be the boss, so re-enter the pend list
    tos_list_del(&task->pend_list);
    // the "someday" comes
    pend_list_add(task, task->pending_obj);
}

__KERNEL__ int pend_object_verify(pend_obj_t *object, pend_type_t type)
{
    return object->type == type;
}

__KERNEL__ k_err_t pend_state2errno(pend_state_t state)
{
    if (state == PEND_STATE_POST) {
        return K_ERR_NONE;
    } else if (state == PEND_STATE_TIMEOUT) {
        return K_ERR_PEND_TIMEOUT;
    } else if (state == PEND_STATE_DESTROY) {
        return K_ERR_PEND_DESTROY;
    } else if (state == PEND_STATE_OWNER_DIE) {
        return K_ERR_PEND_OWNER_DIE;
    } else {
        return K_ERR_PEND_ABNORMAL;
    }
}

__KERNEL__ void pend_task_wakeup(k_task_t *task, pend_state_t state)
{
    if (task_state_is_pending(task)) {
        // mark why we wakeup
        task->pend_state = state;
        pend_list_remove(task);
    }

    if (task_state_is_sleeping(task)) {
        tick_list_remove(task);
    }

    if (task_state_is_suspended(task)) {
        return;
    }

    readyqueue_add(task);
}

__KERNEL__ void pend_task_block(k_task_t *task, pend_obj_t *object, k_tick_t timeout)
{
    readyqueue_remove(task);
    pend_list_add(task, object);

    if (timeout != TOS_TIME_FOREVER) {
        tick_list_add(task, timeout);
    }
}

__KERNEL__ void pend_wakeup_one(pend_obj_t *object, pend_state_t state)
{
    pend_task_wakeup(TOS_LIST_FIRST_ENTRY(&object->list, k_task_t, pend_list), state);
}

__KERNEL__ void pend_wakeup_all(pend_obj_t *object, pend_state_t state)
{
    k_list_t *curr, *next;

    TOS_LIST_FOR_EACH_SAFE(curr, next, &object->list) {
        pend_task_wakeup(TOS_LIST_ENTRY(curr, k_task_t, pend_list), state);
    }
}

__KERNEL__ void pend_wakeup(pend_obj_t *object, pend_state_t state, opt_post_t opt)
{
    if (opt == OPT_POST_ONE) {
        pend_wakeup_one(object, state);
    } else {
        pend_wakeup_all(object, state);
    }
}

