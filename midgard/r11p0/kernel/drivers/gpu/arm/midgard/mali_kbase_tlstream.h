/*
 *
 * (C) COPYRIGHT 2015-2016 ARM Limited. All rights reserved.
 *
 * This program is free software and is provided to you under the terms of the
 * GNU General Public License version 2 as published by the Free Software
 * Foundation, and any use by you of this program is subject to the terms
 * of such GNU licence.
 *
 * A copy of the licence is included with the program, and can also be obtained
 * from Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA  02110-1301, USA.
 *
 */



#if !defined(_KBASE_TLSTREAM_H)
#define _KBASE_TLSTREAM_H

#include <mali_kbase.h>

/*****************************************************************************/

/**
 * kbase_tlstream_init - initialize timeline infrastructure in kernel
 * Return: zero on success, negative number on error
 */
int kbase_tlstream_init(void);

/**
 * kbase_tlstream_term - terminate timeline infrastructure in kernel
 *
 * Timeline need have to been previously enabled with kbase_tlstream_init().
 */
void kbase_tlstream_term(void);

/**
 * kbase_tlstream_acquire - acquire timeline stream file descriptor
 * @kctx: kernel common context
 * @fd:   timeline stream file descriptor
 *
 * This descriptor is meant to be used by userspace timeline to gain access to
 * kernel timeline stream. This stream is later broadcasted by user space to the
 * timeline client.
 * Only one entity can own the descriptor at any given time. Descriptor shall be
 * closed if unused. If descriptor cannot be obtained (i.e. when it is already
 * being used) argument fd will contain negative value.
 *
 * Return: zero on success (this does not necessarily mean that stream
 *         descriptor could be returned), negative number on error
 */
int kbase_tlstream_acquire(struct kbase_context *kctx, int *fd);

/**
 * kbase_tlstream_flush_streams - flush timeline streams.
 *
 * Function will flush pending data in all timeline streams.
 */
void kbase_tlstream_flush_streams(void);

/**
 * kbase_tlstream_reset_body_streams - reset timeline body streams.
 *
 * Function will discard pending data in all timeline body streams.
 */
void kbase_tlstream_reset_body_streams(void);

#if MALI_UNIT_TEST
/**
 * kbase_tlstream_test - start timeline stream data generator
 * @tpw_count: number of trace point writers in each context
 * @msg_delay: time delay in milliseconds between trace points written by one
 *             writer
 * @msg_count: number of trace points written by one writer
 * @aux_msg:   if non-zero aux messages will be included
 *
 * This test starts a requested number of asynchronous writers in both IRQ and
 * thread context. Each writer will generate required number of test
 * tracepoints (tracepoints with embedded information about writer that
 * should be verified by user space reader). Tracepoints will be emitted in
 * all timeline body streams. If aux_msg is non-zero writer will also
 * generate not testable tracepoints (tracepoints without information about
 * writer). These tracepoints are used to check correctness of remaining
 * timeline message generating functions. Writer will wait requested time
 * between generating another set of messages. This call blocks until all
 * writers finish.
 */
void kbase_tlstream_test(
		unsigned int tpw_count,
		unsigned int msg_delay,
		unsigned int msg_count,
		int          aux_msg);

/**
 * kbase_tlstream_stats - read timeline stream statistics
 * @bytes_collected: will hold number of bytes read by the user
 * @bytes_generated: will hold number of bytes generated by trace points
 */
void kbase_tlstream_stats(u32 *bytes_collected, u32 *bytes_generated);
#endif /* MALI_UNIT_TEST */

/*****************************************************************************/

void __kbase_tlstream_tl_summary_new_ctx(void *context, u32 nr, u32 tgid);
void __kbase_tlstream_tl_summary_new_gpu(void *gpu, u32 id, u32 core_count);
void __kbase_tlstream_tl_summary_new_lpu(void *lpu, u32 nr, u32 fn);
void __kbase_tlstream_tl_summary_lifelink_lpu_gpu(void *lpu, void *gpu);
void __kbase_tlstream_tl_summary_new_as(void *as, u32 nr);
void __kbase_tlstream_tl_summary_lifelink_as_gpu(void *as, void *gpu);
void __kbase_tlstream_tl_new_ctx(void *context, u32 nr, u32 tgid);
void __kbase_tlstream_tl_new_atom(void *atom, u32 nr);
void __kbase_tlstream_tl_del_ctx(void *context);
void __kbase_tlstream_tl_del_atom(void *atom);
void __kbase_tlstream_tl_ret_ctx_lpu(void *context, void *lpu);
void __kbase_tlstream_tl_ret_atom_ctx(void *atom, void *context);
void __kbase_tlstream_tl_ret_atom_lpu(
		void *atom, void *lpu, const char *attrib_match_list);
void __kbase_tlstream_tl_nret_ctx_lpu(void *context, void *lpu);
void __kbase_tlstream_tl_nret_atom_ctx(void *atom, void *context);
void __kbase_tlstream_tl_nret_atom_lpu(void *atom, void *lpu);
void __kbase_tlstream_tl_ret_as_ctx(void *as, void *ctx);
void __kbase_tlstream_tl_nret_as_ctx(void *as, void *ctx);
void __kbase_tlstream_tl_ret_atom_as(void *atom, void *as);
void __kbase_tlstream_tl_nret_atom_as(void *atom, void *as);
void __kbase_tlstream_tl_dep_atom_atom(void *atom1, void *atom2);
void __kbase_tlstream_tl_attrib_atom_config(
		void *atom, u64 jd, u64 affinity, u32 config);
void __kbase_tlstream_tl_attrib_as_config(
		void *as, u64 transtab, u64 memattr, u64 transcfg);
void __kbase_tlstream_jd_gpu_soft_reset(void *gpu);
void __kbase_tlstream_aux_pm_state(u32 core_type, u64 state);
void __kbase_tlstream_aux_issue_job_softstop(void *katom);
void __kbase_tlstream_aux_job_softstop(u32 js_id);
void __kbase_tlstream_aux_job_softstop_ex(struct kbase_jd_atom *katom);
void __kbase_tlstream_aux_pagefault(u32 ctx_nr, u64 page_count_change);
void __kbase_tlstream_aux_pagesalloc(u32 ctx_nr, u64 page_count);

extern atomic_t kbase_tlstream_enabled;

#define __TRACE_IF_ENABLED(trace_name, ...)                         \
	do {                                                        \
		int enabled = atomic_read(&kbase_tlstream_enabled); \
		if (enabled)                                        \
			__kbase_tlstream_##trace_name(__VA_ARGS__); \
	} while (0)

/*****************************************************************************/

/**
 * kbase_tlstream_tl_summary_new_ctx - create context object in timeline
 *                                     summary
 * @context: name of the context object
 * @nr:      context number
 * @tgid:    thread Group Id
 *
 * Function emits a timeline message informing about context creation. Context
 * is created with context number (its attribute), that can be used to link
 * kbase context with userspace context.
 * This message is directed to timeline summary stream.
 */
#define kbase_tlstream_tl_summary_new_ctx(context, nr, tgid) \
	__TRACE_IF_ENABLED(tl_summary_new_ctx, context, nr, tgid)

/**
 * kbase_tlstream_tl_summary_new_gpu - create GPU object in timeline summary
 * @gpu:        name of the GPU object
 * @id:         id value of this GPU
 * @core_count: number of cores this GPU hosts
 *
 * Function emits a timeline message informing about GPU creation. GPU is
 * created with two attributes: id and core count.
 * This message is directed to timeline summary stream.
 */
#define kbase_tlstream_tl_summary_new_gpu(gpu, id, core_count) \
	__TRACE_IF_ENABLED(tl_summary_new_gpu, gpu, id, core_count)

/**
 * kbase_tlstream_tl_summary_new_lpu - create LPU object in timeline summary
 * @lpu: name of the Logical Processing Unit object
 * @nr:  sequential number assigned to this LPU
 * @fn:  property describing this LPU's functional abilities
 *
 * Function emits a timeline message informing about LPU creation. LPU is
 * created with two attributes: number linking this LPU with GPU's job slot
 * and function bearing information about this LPU abilities.
 * This message is directed to timeline summary stream.
 */
#define kbase_tlstream_tl_summary_new_lpu(lpu, nr, fn) \
	__TRACE_IF_ENABLED(tl_summary_new_lpu, lpu, nr, fn)

/**
 * kbase_tlstream_tl_summary_lifelink_lpu_gpu - lifelink LPU object to GPU
 * @lpu: name of the Logical Processing Unit object
 * @gpu: name of the GPU object
 *
 * Function emits a timeline message informing that LPU object shall be deleted
 * along with GPU object.
 * This message is directed to timeline summary stream.
 */
#define kbase_tlstream_tl_summary_lifelink_lpu_gpu(lpu, gpu) \
	__TRACE_IF_ENABLED(tl_summary_lifelink_lpu_gpu, lpu, gpu)

/**
 * kbase_tlstream_tl_summary_new_as - create address space object in timeline summary
 * @as: name of the address space object
 * @nr: sequential number assigned to this address space
 *
 * Function emits a timeline message informing about address space creation.
 * Address space is created with one attribute: number identifying this
 * address space.
 * This message is directed to timeline summary stream.
 */
#define kbase_tlstream_tl_summary_new_as(as, nr) \
	__TRACE_IF_ENABLED(tl_summary_new_as, as, nr)

/**
 * kbase_tlstream_tl_summary_lifelink_as_gpu - lifelink address space object to GPU
 * @as:  name of the address space object
 * @gpu: name of the GPU object
 *
 * Function emits a timeline message informing that address space object
 * shall be deleted along with GPU object.
 * This message is directed to timeline summary stream.
 */
#define kbase_tlstream_tl_summary_lifelink_as_gpu(as, gpu) \
	__TRACE_IF_ENABLED(tl_summary_lifelink_as_gpu, as, gpu)

/**
 * kbase_tlstream_tl_new_ctx - create context object in timeline
 * @context: name of the context object
 * @nr:      context number
 * @tgid:    thread Group Id
 *
 * Function emits a timeline message informing about context creation. Context
 * is created with context number (its attribute), that can be used to link
 * kbase context with userspace context.
 */
#define kbase_tlstream_tl_new_ctx(context, nr, tgid) \
	__TRACE_IF_ENABLED(tl_new_ctx, context, nr, tgid)

/**
 * kbase_tlstream_tl_new_atom - create atom object in timeline
 * @atom: name of the atom object
 * @nr:   sequential number assigned to this atom
 *
 * Function emits a timeline message informing about atom creation. Atom is
 * created with atom number (its attribute) that links it with actual work
 * bucket id understood by hardware.
 */
#define kbase_tlstream_tl_new_atom(atom, nr) \
	__TRACE_IF_ENABLED(tl_new_atom, atom, nr)

/**
 * kbase_tlstream_tl_del_ctx - destroy context object in timeline
 * @context: name of the context object
 *
 * Function emits a timeline message informing that context object ceased to
 * exist.
 */
#define kbase_tlstream_tl_del_ctx(context) \
	__TRACE_IF_ENABLED(tl_del_ctx, context)

/**
 * kbase_tlstream_tl_del_atom - destroy atom object in timeline
 * @atom: name of the atom object
 *
 * Function emits a timeline message informing that atom object ceased to
 * exist.
 */
#define kbase_tlstream_tl_del_atom(atom) \
	__TRACE_IF_ENABLED(tl_del_atom, atom)

/**
 * kbase_tlstream_tl_ret_ctx_lpu - retain context by LPU
 * @context: name of the context object
 * @lpu:     name of the Logical Processing Unit object
 *
 * Function emits a timeline message informing that context is being held
 * by LPU and must not be deleted unless it is released.
 */
#define kbase_tlstream_tl_ret_ctx_lpu(context, lpu) \
	__TRACE_IF_ENABLED(tl_ret_ctx_lpu, context, lpu)

/**
 * kbase_tlstream_tl_ret_atom_ctx - retain atom by context
 * @atom:    name of the atom object
 * @context: name of the context object
 *
 * Function emits a timeline message informing that atom object is being held
 * by context and must not be deleted unless it is released.
 */
#define kbase_tlstream_tl_ret_atom_ctx(atom, context) \
	__TRACE_IF_ENABLED(tl_ret_atom_ctx, atom, context)

/**
 * kbase_tlstream_tl_ret_atom_lpu - retain atom by LPU
 * @atom:              name of the atom object
 * @lpu:               name of the Logical Processing Unit object
 * @attrib_match_list: list containing match operator attributes
 *
 * Function emits a timeline message informing that atom object is being held
 * by LPU and must not be deleted unless it is released.
 */
#define kbase_tlstream_tl_ret_atom_lpu(atom, lpu, attrib_match_list) \
	__TRACE_IF_ENABLED(tl_ret_atom_lpu, atom, lpu, attrib_match_list)

/**
 * kbase_tlstream_tl_nret_ctx_lpu - release context by LPU
 * @context: name of the context object
 * @lpu:     name of the Logical Processing Unit object
 *
 * Function emits a timeline message informing that context is being released
 * by LPU object.
 */
#define kbase_tlstream_tl_nret_ctx_lpu(context, lpu) \
	__TRACE_IF_ENABLED(tl_nret_ctx_lpu, context, lpu)

/**
 * kbase_tlstream_tl_nret_atom_ctx - release atom by context
 * @atom:    name of the atom object
 * @context: name of the context object
 *
 * Function emits a timeline message informing that atom object is being
 * released by context.
 */
#define kbase_tlstream_tl_nret_atom_ctx(atom, context) \
	__TRACE_IF_ENABLED(tl_nret_atom_ctx, atom, context)

/**
 * kbase_tlstream_tl_nret_atom_lpu - release atom by LPU
 * @atom: name of the atom object
 * @lpu:  name of the Logical Processing Unit object
 *
 * Function emits a timeline message informing that atom object is being
 * released by LPU.
 */
#define kbase_tlstream_tl_nret_atom_lpu(atom, lpu) \
	__TRACE_IF_ENABLED(tl_nret_atom_lpu, atom, lpu)

/**
 * kbase_tlstream_tl_ret_as_ctx - lifelink address space object to context
 * @as:  name of the address space object
 * @ctx: name of the context object
 *
 * Function emits a timeline message informing that address space object
 * is being held by the context object.
 */
#define kbase_tlstream_tl_ret_as_ctx(as, ctx) \
	__TRACE_IF_ENABLED(tl_ret_as_ctx, as, ctx)

/**
 * kbase_tlstream_tl_nret_as_ctx - release address space by context
 * @as:  name of the address space object
 * @ctx: name of the context object
 *
 * Function emits a timeline message informing that address space object
 * is being released by atom.
 */
#define kbase_tlstream_tl_nret_as_ctx(as, ctx) \
	__TRACE_IF_ENABLED(tl_nret_as_ctx, as, ctx)

/**
 * kbase_tlstream_tl_ret_atom_as - retain atom by address space
 * @atom: name of the atom object
 * @as:   name of the address space object
 *
 * Function emits a timeline message informing that atom object is being held
 * by address space and must not be deleted unless it is released.
 */
#define kbase_tlstream_tl_ret_atom_as(atom, as) \
	__TRACE_IF_ENABLED(tl_ret_atom_as, atom, as)

/**
 * kbase_tlstream_tl_nret_atom_as - release atom by address space
 * @atom: name of the atom object
 * @as:   name of the address space object
 *
 * Function emits a timeline message informing that atom object is being
 * released by address space.
 */
#define kbase_tlstream_tl_nret_atom_as(atom, as) \
	__TRACE_IF_ENABLED(tl_nret_atom_as, atom, as)

/**
 * kbase_tlstream_tl_dep_atom_atom - parent atom depends on child atom
 * @atom1: name of the child atom object
 * @atom2: name of the parent atom object that depends on child atom
 *
 * Function emits a timeline message informing that parent atom waits for
 * child atom object to be completed before start its execution.
 */
#define kbase_tlstream_tl_dep_atom_atom(atom1, atom2) \
	__TRACE_IF_ENABLED(tl_dep_atom_atom, atom1, atom2)

/**
 * kbase_tlstream_tl_attrib_atom_config - atom job slot attributes
 * @atom:     name of the atom object
 * @jd:       job descriptor address
 * @affinity: job affinity
 * @config:   job config
 *
 * Function emits a timeline message containing atom attributes.
 */
#define kbase_tlstream_tl_attrib_atom_config(atom, jd, affinity, config) \
	__TRACE_IF_ENABLED(tl_attrib_atom_config, atom, jd, affinity, config)

/**
 * kbase_tlstream_tl_attrib_as_config - address space attributes
 * @as:       assigned address space
 * @transtab: configuration of the TRANSTAB register
 * @memattr:  configuration of the MEMATTR register
 * @transcfg: configuration of the TRANSCFG register (or zero if not present)
 *
 * Function emits a timeline message containing address space attributes.
 */
#define kbase_tlstream_tl_attrib_as_config(as, transtab, memattr, transcfg) \
	__TRACE_IF_ENABLED(tl_attrib_as_config, as, transtab, memattr, transcfg)

/**
 * kbase_tlstream_jd_gpu_soft_reset - The GPU is being soft reset
 * @gpu:        name of the GPU object
 *
 * This imperative tracepoint is specific to job dumping.
 * Function emits a timeline message indicating GPU soft reset.
 */
#define kbase_tlstream_jd_gpu_soft_reset(gpu) \
	__TRACE_IF_ENABLED(jd_gpu_soft_reset, gpu)

/**
 * kbase_tlstream_aux_pm_state - timeline message: power management state
 * @core_type: core type (shader, tiler, l2 cache, l3 cache)
 * @state:     64bits bitmask reporting power state of the cores (1-ON, 0-OFF)
 */
#define kbase_tlstream_aux_pm_state(core_type, state) \
	__TRACE_IF_ENABLED(aux_pm_state, core_type, state)

/**
 * kbase_tlstream_aux_issue_job_softstop - a soft-stop command is being issued
 * @katom: the atom that is being soft-stopped
 */
#define kbase_tlstream_aux_issue_job_softstop(katom) \
	__TRACE_IF_ENABLED(aux_issue_job_softstop, katom)

/**
 * kbase_tlstream_aux_job_softstop - soft job stop occurred
 * @js_id: job slot id
 */
#define kbase_tlstream_aux_job_softstop(js_id) \
	__TRACE_IF_ENABLED(aux_job_softstop, js_id)

/**
 * kbase_tlstream_aux_job_softstop_ex - extra info about soft-stopped atom
 * @katom: the atom that has been soft-stopped
 *
 * This trace point adds more details about the soft-stopped atom. These details
 * can't be safety collected inside the interrupt handler so we're doing it
 * inside a worker.
 *
 * Note: this is not the same information that is recorded in the trace point,
 * refer to __kbase_tlstream_aux_job_softstop_ex() for more details.
 */
#define kbase_tlstream_aux_job_softstop_ex(katom) \
	__TRACE_IF_ENABLED(aux_job_softstop_ex, katom)

/**
 * kbase_tlstream_aux_pagefault - timeline message: MMU page fault event
 *                                resulting in new pages being mapped
 * @ctx_nr:            kernel context number
 * @page_count_change: number of pages to be added
 */
#define kbase_tlstream_aux_pagefault(ctx_nr, page_count_change) \
	__TRACE_IF_ENABLED(aux_pagefault, ctx_nr, page_count_change)

/**
 * kbase_tlstream_aux_pagesalloc - timeline message: total number of allocated
 *                                 pages is changed
 * @ctx_nr:     kernel context number
 * @page_count: number of pages used by the context
 */
#define kbase_tlstream_aux_pagesalloc(ctx_nr, page_count) \
	__TRACE_IF_ENABLED(aux_pagesalloc, ctx_nr, page_count)

#endif /* _KBASE_TLSTREAM_H */

