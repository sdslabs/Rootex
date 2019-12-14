//this file is autogenerated using stringify.bat (premake --stringify) in the build folder of this project
static const char* batchingKernelsCL = "/*\n"
                                       "Copyright (c) 2012 Advanced Micro Devices, Inc.  \n"
                                       "This software is provided 'as-is', without any express or implied warranty.\n"
                                       "In no event will the authors be held liable for any damages arising from the use of this software.\n"
                                       "Permission is granted to anyone to use this software for any purpose, \n"
                                       "including commercial applications, and to alter it and redistribute it freely, \n"
                                       "subject to the following restrictions:\n"
                                       "1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.\n"
                                       "2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.\n"
                                       "3. This notice may not be removed or altered from any source distribution.\n"
                                       "*/\n"
                                       "//Originally written by Takahiro Harada\n"
                                       "#ifndef B3_CONTACT4DATA_H\n"
                                       "#define B3_CONTACT4DATA_H\n"
                                       "#ifndef B3_FLOAT4_H\n"
                                       "#define B3_FLOAT4_H\n"
                                       "#ifndef B3_PLATFORM_DEFINITIONS_H\n"
                                       "#define B3_PLATFORM_DEFINITIONS_H\n"
                                       "struct MyTest\n"
                                       "{\n"
                                       "	int bla;\n"
                                       "};\n"
                                       "#ifdef __cplusplus\n"
                                       "#else\n"
                                       "//keep B3_LARGE_FLOAT*B3_LARGE_FLOAT < FLT_MAX\n"
                                       "#define B3_LARGE_FLOAT 1e18f\n"
                                       "#define B3_INFINITY 1e18f\n"
                                       "#define b3Assert(a)\n"
                                       "#define b3ConstArray(a) __global const a*\n"
                                       "#define b3AtomicInc atomic_inc\n"
                                       "#define b3AtomicAdd atomic_add\n"
                                       "#define b3Fabs fabs\n"
                                       "#define b3Sqrt native_sqrt\n"
                                       "#define b3Sin native_sin\n"
                                       "#define b3Cos native_cos\n"
                                       "#define B3_STATIC\n"
                                       "#endif\n"
                                       "#endif\n"
                                       "#ifdef __cplusplus\n"
                                       "#else\n"
                                       "	typedef float4	b3Float4;\n"
                                       "	#define b3Float4ConstArg const b3Float4\n"
                                       "	#define b3MakeFloat4 (float4)\n"
                                       "	float b3Dot3F4(b3Float4ConstArg v0,b3Float4ConstArg v1)\n"
                                       "	{\n"
                                       "		float4 a1 = b3MakeFloat4(v0.xyz,0.f);\n"
                                       "		float4 b1 = b3MakeFloat4(v1.xyz,0.f);\n"
                                       "		return dot(a1, b1);\n"
                                       "	}\n"
                                       "	b3Float4 b3Cross3(b3Float4ConstArg v0,b3Float4ConstArg v1)\n"
                                       "	{\n"
                                       "		float4 a1 = b3MakeFloat4(v0.xyz,0.f);\n"
                                       "		float4 b1 = b3MakeFloat4(v1.xyz,0.f);\n"
                                       "		return cross(a1, b1);\n"
                                       "	}\n"
                                       "	#define b3MinFloat4 min\n"
                                       "	#define b3MaxFloat4 max\n"
                                       "	#define b3Normalized(a) normalize(a)\n"
                                       "#endif \n"
                                       "		\n"
                                       "inline bool b3IsAlmostZero(b3Float4ConstArg v)\n"
                                       "{\n"
                                       "	if(b3Fabs(v.x)>1e-6 || b3Fabs(v.y)>1e-6 || b3Fabs(v.z)>1e-6)	\n"
                                       "		return false;\n"
                                       "	return true;\n"
                                       "}\n"
                                       "inline int    b3MaxDot( b3Float4ConstArg vec, __global const b3Float4* vecArray, int vecLen, float* dotOut )\n"
                                       "{\n"
                                       "    float maxDot = -B3_INFINITY;\n"
                                       "    int i = 0;\n"
                                       "    int ptIndex = -1;\n"
                                       "    for( i = 0; i < vecLen; i++ )\n"
                                       "    {\n"
                                       "        float dot = b3Dot3F4(vecArray[i],vec);\n"
                                       "            \n"
                                       "        if( dot > maxDot )\n"
                                       "        {\n"
                                       "            maxDot = dot;\n"
                                       "            ptIndex = i;\n"
                                       "        }\n"
                                       "    }\n"
                                       "	b3Assert(ptIndex>=0);\n"
                                       "    if (ptIndex<0)\n"
                                       "	{\n"
                                       "		ptIndex = 0;\n"
                                       "	}\n"
                                       "    *dotOut = maxDot;\n"
                                       "    return ptIndex;\n"
                                       "}\n"
                                       "#endif //B3_FLOAT4_H\n"
                                       "typedef  struct b3Contact4Data b3Contact4Data_t;\n"
                                       "struct b3Contact4Data\n"
                                       "{\n"
                                       "	b3Float4	m_worldPosB[4];\n"
                                       "//	b3Float4	m_localPosA[4];\n"
                                       "//	b3Float4	m_localPosB[4];\n"
                                       "	b3Float4	m_worldNormalOnB;	//	w: m_nPoints\n"
                                       "	unsigned short  m_restituitionCoeffCmp;\n"
                                       "	unsigned short  m_frictionCoeffCmp;\n"
                                       "	int m_batchIdx;\n"
                                       "	int m_bodyAPtrAndSignBit;//x:m_bodyAPtr, y:m_bodyBPtr\n"
                                       "	int m_bodyBPtrAndSignBit;\n"
                                       "	int	m_childIndexA;\n"
                                       "	int	m_childIndexB;\n"
                                       "	int m_unused1;\n"
                                       "	int m_unused2;\n"
                                       "};\n"
                                       "inline int b3Contact4Data_getNumPoints(const struct b3Contact4Data* contact)\n"
                                       "{\n"
                                       "	return (int)contact->m_worldNormalOnB.w;\n"
                                       "};\n"
                                       "inline void b3Contact4Data_setNumPoints(struct b3Contact4Data* contact, int numPoints)\n"
                                       "{\n"
                                       "	contact->m_worldNormalOnB.w = (float)numPoints;\n"
                                       "};\n"
                                       "#endif //B3_CONTACT4DATA_H\n"
                                       "#pragma OPENCL EXTENSION cl_amd_printf : enable\n"
                                       "#pragma OPENCL EXTENSION cl_khr_local_int32_base_atomics : enable\n"
                                       "#pragma OPENCL EXTENSION cl_khr_global_int32_base_atomics : enable\n"
                                       "#pragma OPENCL EXTENSION cl_khr_local_int32_extended_atomics : enable\n"
                                       "#pragma OPENCL EXTENSION cl_khr_global_int32_extended_atomics : enable\n"
                                       "#ifdef cl_ext_atomic_counters_32\n"
                                       "#pragma OPENCL EXTENSION cl_ext_atomic_counters_32 : enable\n"
                                       "#else\n"
                                       "#define counter32_t volatile __global int*\n"
                                       "#endif\n"
                                       "typedef unsigned int u32;\n"
                                       "typedef unsigned short u16;\n"
                                       "typedef unsigned char u8;\n"
                                       "#define GET_GROUP_IDX get_group_id(0)\n"
                                       "#define GET_LOCAL_IDX get_local_id(0)\n"
                                       "#define GET_GLOBAL_IDX get_global_id(0)\n"
                                       "#define GET_GROUP_SIZE get_local_size(0)\n"
                                       "#define GET_NUM_GROUPS get_num_groups(0)\n"
                                       "#define GROUP_LDS_BARRIER barrier(CLK_LOCAL_MEM_FENCE)\n"
                                       "#define GROUP_MEM_FENCE mem_fence(CLK_LOCAL_MEM_FENCE)\n"
                                       "#define AtomInc(x) atom_inc(&(x))\n"
                                       "#define AtomInc1(x, out) out = atom_inc(&(x))\n"
                                       "#define AppendInc(x, out) out = atomic_inc(x)\n"
                                       "#define AtomAdd(x, value) atom_add(&(x), value)\n"
                                       "#define AtomCmpxhg(x, cmp, value) atom_cmpxchg( &(x), cmp, value )\n"
                                       "#define AtomXhg(x, value) atom_xchg ( &(x), value )\n"
                                       "#define SELECT_UINT4( b, a, condition ) select( b,a,condition )\n"
                                       "#define make_float4 (float4)\n"
                                       "#define make_float2 (float2)\n"
                                       "#define make_uint4 (uint4)\n"
                                       "#define make_int4 (int4)\n"
                                       "#define make_uint2 (uint2)\n"
                                       "#define make_int2 (int2)\n"
                                       "#define max2 max\n"
                                       "#define min2 min\n"
                                       "#define WG_SIZE 64\n"
                                       "typedef struct \n"
                                       "{\n"
                                       "	int m_n;\n"
                                       "	int m_start;\n"
                                       "	int m_staticIdx;\n"
                                       "	int m_paddings[1];\n"
                                       "} ConstBuffer;\n"
                                       "typedef struct \n"
                                       "{\n"
                                       "	int m_a;\n"
                                       "	int m_b;\n"
                                       "	u32 m_idx;\n"
                                       "}Elem;\n"
                                       "#define STACK_SIZE (WG_SIZE*10)\n"
                                       "//#define STACK_SIZE (WG_SIZE)\n"
                                       "#define RING_SIZE 1024\n"
                                       "#define RING_SIZE_MASK (RING_SIZE-1)\n"
                                       "#define CHECK_SIZE (WG_SIZE)\n"
                                       "#define GET_RING_CAPACITY (RING_SIZE - ldsRingEnd)\n"
                                       "#define RING_END ldsTmp\n"
                                       "u32 readBuf(__local u32* buff, int idx)\n"
                                       "{\n"
                                       "	idx = idx % (32*CHECK_SIZE);\n"
                                       "	int bitIdx = idx%32;\n"
                                       "	int bufIdx = idx/32;\n"
                                       "	return buff[bufIdx] & (1<<bitIdx);\n"
                                       "}\n"
                                       "void writeBuf(__local u32* buff, int idx)\n"
                                       "{\n"
                                       "	idx = idx % (32*CHECK_SIZE);\n"
                                       "	int bitIdx = idx%32;\n"
                                       "	int bufIdx = idx/32;\n"
                                       "//	buff[bufIdx] |= (1<<bitIdx);\n"
                                       "	atom_or( &buff[bufIdx], (1<<bitIdx) );\n"
                                       "}\n"
                                       "u32 tryWrite(__local u32* buff, int idx)\n"
                                       "{\n"
                                       "	idx = idx % (32*CHECK_SIZE);\n"
                                       "	int bitIdx = idx%32;\n"
                                       "	int bufIdx = idx/32;\n"
                                       "	u32 ans = (u32)atom_or( &buff[bufIdx], (1<<bitIdx) );\n"
                                       "	return ((ans >> bitIdx)&1) == 0;\n"
                                       "}\n"
                                       "//	batching on the GPU\n"
                                       "__kernel void CreateBatches( __global const struct b3Contact4Data* gConstraints, __global struct b3Contact4Data* gConstraintsOut,\n"
                                       "		__global const u32* gN, __global const u32* gStart, __global int* batchSizes, \n"
                                       "		int m_staticIdx )\n"
                                       "{\n"
                                       "	__local u32 ldsStackIdx[STACK_SIZE];\n"
                                       "	__local u32 ldsStackEnd;\n"
                                       "	__local Elem ldsRingElem[RING_SIZE];\n"
                                       "	__local u32 ldsRingEnd;\n"
                                       "	__local u32 ldsTmp;\n"
                                       "	__local u32 ldsCheckBuffer[CHECK_SIZE];\n"
                                       "	__local u32 ldsFixedBuffer[CHECK_SIZE];\n"
                                       "	__local u32 ldsGEnd;\n"
                                       "	__local u32 ldsDstEnd;\n"
                                       "	int wgIdx = GET_GROUP_IDX;\n"
                                       "	int lIdx = GET_LOCAL_IDX;\n"
                                       "	\n"
                                       "	const int m_n = gN[wgIdx];\n"
                                       "	const int m_start = gStart[wgIdx];\n"
                                       "		\n"
                                       "	if( lIdx == 0 )\n"
                                       "	{\n"
                                       "		ldsRingEnd = 0;\n"
                                       "		ldsGEnd = 0;\n"
                                       "		ldsStackEnd = 0;\n"
                                       "		ldsDstEnd = m_start;\n"
                                       "	}\n"
                                       "	\n"
                                       "	\n"
                                       "	\n"
                                       "//	while(1)\n"
                                       "//was 250\n"
                                       "	int ie=0;\n"
                                       "	int maxBatch = 0;\n"
                                       "	for(ie=0; ie<50; ie++)\n"
                                       "	{\n"
                                       "		ldsFixedBuffer[lIdx] = 0;\n"
                                       "		for(int giter=0; giter<4; giter++)\n"
                                       "		{\n"
                                       "			int ringCap = GET_RING_CAPACITY;\n"
                                       "		\n"
                                       "			//	1. fill ring\n"
                                       "			if( ldsGEnd < m_n )\n"
                                       "			{\n"
                                       "				while( ringCap > WG_SIZE )\n"
                                       "				{\n"
                                       "					if( ldsGEnd >= m_n ) break;\n"
                                       "					if( lIdx < ringCap - WG_SIZE )\n"
                                       "					{\n"
                                       "						int srcIdx;\n"
                                       "						AtomInc1( ldsGEnd, srcIdx );\n"
                                       "						if( srcIdx < m_n )\n"
                                       "						{\n"
                                       "							int dstIdx;\n"
                                       "							AtomInc1( ldsRingEnd, dstIdx );\n"
                                       "							\n"
                                       "							int a = gConstraints[m_start+srcIdx].m_bodyAPtrAndSignBit;\n"
                                       "							int b = gConstraints[m_start+srcIdx].m_bodyBPtrAndSignBit;\n"
                                       "							ldsRingElem[dstIdx].m_a = (a>b)? b:a;\n"
                                       "							ldsRingElem[dstIdx].m_b = (a>b)? a:b;\n"
                                       "							ldsRingElem[dstIdx].m_idx = srcIdx;\n"
                                       "						}\n"
                                       "					}\n"
                                       "					ringCap = GET_RING_CAPACITY;\n"
                                       "				}\n"
                                       "			}\n"
                                       "			GROUP_LDS_BARRIER;\n"
                                       "	\n"
                                       "			//	2. fill stack\n"
                                       "			__local Elem* dst = ldsRingElem;\n"
                                       "			if( lIdx == 0 ) RING_END = 0;\n"
                                       "			int srcIdx=lIdx;\n"
                                       "			int end = ldsRingEnd;\n"
                                       "			{\n"
                                       "				for(int ii=0; ii<end; ii+=WG_SIZE, srcIdx+=WG_SIZE)\n"
                                       "				{\n"
                                       "					Elem e;\n"
                                       "					if(srcIdx<end) e = ldsRingElem[srcIdx];\n"
                                       "					bool done = (srcIdx<end)?false:true;\n"
                                       "					for(int i=lIdx; i<CHECK_SIZE; i+=WG_SIZE) ldsCheckBuffer[lIdx] = 0;\n"
                                       "					\n"
                                       "					if( !done )\n"
                                       "					{\n"
                                       "						int aUsed = readBuf( ldsFixedBuffer, abs(e.m_a));\n"
                                       "						int bUsed = readBuf( ldsFixedBuffer, abs(e.m_b));\n"
                                       "						if( aUsed==0 && bUsed==0 )\n"
                                       "						{\n"
                                       "							int aAvailable=1;\n"
                                       "							int bAvailable=1;\n"
                                       "							int ea = abs(e.m_a);\n"
                                       "							int eb = abs(e.m_b);\n"
                                       "							bool aStatic = (e.m_a<0) ||(ea==m_staticIdx);\n"
                                       "							bool bStatic = (e.m_b<0) ||(eb==m_staticIdx);\n"
                                       "							\n"
                                       "							if (!aStatic)\n"
                                       "								aAvailable = tryWrite( ldsCheckBuffer, ea );\n"
                                       "							if (!bStatic)\n"
                                       "								bAvailable = tryWrite( ldsCheckBuffer, eb );\n"
                                       "							\n"
                                       "							//aAvailable = aStatic? 1: aAvailable;\n"
                                       "							//bAvailable = bStatic? 1: bAvailable;\n"
                                       "							bool success = (aAvailable && bAvailable);\n"
                                       "							if(success)\n"
                                       "							{\n"
                                       "							\n"
                                       "								if (!aStatic)\n"
                                       "									writeBuf( ldsFixedBuffer, ea );\n"
                                       "								if (!bStatic)\n"
                                       "									writeBuf( ldsFixedBuffer, eb );\n"
                                       "							}\n"
                                       "							done = success;\n"
                                       "						}\n"
                                       "					}\n"
                                       "					//	put it aside\n"
                                       "					if(srcIdx<end)\n"
                                       "					{\n"
                                       "						if( done )\n"
                                       "						{\n"
                                       "							int dstIdx; AtomInc1( ldsStackEnd, dstIdx );\n"
                                       "							if( dstIdx < STACK_SIZE )\n"
                                       "								ldsStackIdx[dstIdx] = e.m_idx;\n"
                                       "							else{\n"
                                       "								done = false;\n"
                                       "								AtomAdd( ldsStackEnd, -1 );\n"
                                       "							}\n"
                                       "						}\n"
                                       "						if( !done )\n"
                                       "						{\n"
                                       "							int dstIdx; AtomInc1( RING_END, dstIdx );\n"
                                       "							dst[dstIdx] = e;\n"
                                       "						}\n"
                                       "					}\n"
                                       "					//	if filled, flush\n"
                                       "					if( ldsStackEnd == STACK_SIZE )\n"
                                       "					{\n"
                                       "						for(int i=lIdx; i<STACK_SIZE; i+=WG_SIZE)\n"
                                       "						{\n"
                                       "							int idx = m_start + ldsStackIdx[i];\n"
                                       "							int dstIdx; AtomInc1( ldsDstEnd, dstIdx );\n"
                                       "							gConstraintsOut[ dstIdx ] = gConstraints[ idx ];\n"
                                       "							gConstraintsOut[ dstIdx ].m_batchIdx = ie;\n"
                                       "						}\n"
                                       "						if( lIdx == 0 ) ldsStackEnd = 0;\n"
                                       "						//for(int i=lIdx; i<CHECK_SIZE; i+=WG_SIZE) \n"
                                       "						ldsFixedBuffer[lIdx] = 0;\n"
                                       "					}\n"
                                       "				}\n"
                                       "			}\n"
                                       "			if( lIdx == 0 ) ldsRingEnd = RING_END;\n"
                                       "		}\n"
                                       "		GROUP_LDS_BARRIER;\n"
                                       "		for(int i=lIdx; i<ldsStackEnd; i+=WG_SIZE)\n"
                                       "		{\n"
                                       "			int idx = m_start + ldsStackIdx[i];\n"
                                       "			int dstIdx; AtomInc1( ldsDstEnd, dstIdx );\n"
                                       "			gConstraintsOut[ dstIdx ] = gConstraints[ idx ];\n"
                                       "			gConstraintsOut[ dstIdx ].m_batchIdx = ie;\n"
                                       "		}\n"
                                       "		//	in case it couldn't consume any pair. Flush them\n"
                                       "		//	todo. Serial batch worth while?\n"
                                       "		if( ldsStackEnd == 0 )\n"
                                       "		{\n"
                                       "			for(int i=lIdx; i<ldsRingEnd; i+=WG_SIZE)\n"
                                       "			{\n"
                                       "				int idx = m_start + ldsRingElem[i].m_idx;\n"
                                       "				int dstIdx; AtomInc1( ldsDstEnd, dstIdx );\n"
                                       "				gConstraintsOut[ dstIdx ] = gConstraints[ idx ];\n"
                                       "				int curBatch = 100+i;\n"
                                       "				if (maxBatch < curBatch)\n"
                                       "					maxBatch = curBatch;\n"
                                       "				\n"
                                       "				gConstraintsOut[ dstIdx ].m_batchIdx = curBatch;\n"
                                       "				\n"
                                       "			}\n"
                                       "			GROUP_LDS_BARRIER;\n"
                                       "			if( lIdx == 0 ) ldsRingEnd = 0;\n"
                                       "		}\n"
                                       "		if( lIdx == 0 ) ldsStackEnd = 0;\n"
                                       "		GROUP_LDS_BARRIER;\n"
                                       "		//	termination\n"
                                       "		if( ldsGEnd == m_n && ldsRingEnd == 0 )\n"
                                       "			break;\n"
                                       "	}\n"
                                       "	if( lIdx == 0 )\n"
                                       "	{\n"
                                       "		if (maxBatch < ie)\n"
                                       "			maxBatch=ie;\n"
                                       "		batchSizes[wgIdx]=maxBatch;\n"
                                       "	}\n"
                                       "}\n";
