// This file has been automatically generated. Do not change it manually, rather look
// for the template in qphix-codegen.

#pragma once

#include "qphix/diagnostics.h"

#if defined(QPHIX_AVX_SOURCE)
QPHIX_MESSAGE("including qphix/avx/dslash_avx_complete_specialization.h")
#include "qphix/avx/dslash_avx_complete_specialization.h"
#endif

#if defined(QPHIX_AVX2_SOURCE)
QPHIX_MESSAGE("including qphix/avx2/dslash_avx2_complete_specialization.h")
#include "qphix/avx2/dslash_avx2_complete_specialization.h"
#endif

#if defined(QPHIX_AVX512_SOURCE)
QPHIX_MESSAGE("including qphix/avx512/dslash_avx512_complete_specialization.h")
#include "qphix/avx512/dslash_avx512_complete_specialization.h"
#endif

#if defined(QPHIX_MIC_SOURCE)
QPHIX_MESSAGE("including qphix/mic/dslash_mic_complete_specialization.h")
#include "qphix/mic/dslash_mic_complete_specialization.h"
#endif

#if defined(QPHIX_QPX_SOURCE)
QPHIX_MESSAGE("including qphix/qpx/dslash_qpx_complete_specialization.h")
#include "qphix/qpx/dslash_qpx_complete_specialization.h"
#endif

#if defined(QPHIX_SCALAR_SOURCE)
QPHIX_MESSAGE("including qphix/scalar/dslash_scalar_complete_specialization.h")
#include "qphix/scalar/dslash_scalar_complete_specialization.h"
#endif

#if defined(QPHIX_SSE_SOURCE)
QPHIX_MESSAGE("including qphix/sse/dslash_sse_complete_specialization.h")
#include "qphix/sse/dslash_sse_complete_specialization.h"
#endif
