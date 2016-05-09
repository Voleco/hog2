#include "PEMMRubik.h"

void PEMMRubik::GetState(RubiksState & s, int bucket, uint64_t data)
{
	RubikCornerPDB::GetStateFromHash(s.corner, data%RubikCornerPDB::GetStateSpaceSize());
	RubikEdgePDB::GetStateFromHash(s.edge, bucket | ((data / RubikCornerPDB::GetStateSpaceSize()) << PEMM<RubiksState, RubiksAction>::bucketBits));
}

int PEMMRubik::GetBucket(const RubiksState & s)
{
	uint64_t ehash = RubikEdgePDB::GetStateHash(s.edge);
	return ehash & PEMM<RubiksState, RubiksAction>::bucketMask;
}

void PEMMRubik::GetBucketAndData(const RubiksState & s, int & bucket, uint64_t & data)
{
	uint64_t ehash = RubikEdgePDB::GetStateHash(s.edge);
	uint64_t chash = RubikCornerPDB::GetStateHash(s.corner);
	bucket = ehash & PEMM<RubiksState, RubiksAction>::bucketMask;
	data = (ehash >> PEMM<RubiksState, RubiksAction>::bucketBits)*RubikCornerPDB::GetStateSpaceSize() + chash;
}