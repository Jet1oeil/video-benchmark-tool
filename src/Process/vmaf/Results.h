#ifndef VMAF_RESULTS_H_
#define VMAF_RESULTS_H_

namespace vmaf {
	struct Results {
		int iBitstreamSize;
		int dEncodingTime;
		int dDecdodingTime;
		double dVMAFScore;
	};
}

#endif // VMAF_RESULTS_H_
