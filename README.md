# Video Benchmark Tool

Video Benchmark Tool is an open-source software to benchmark the video processing performances of a target machine. We provide an GUI interface or CLI interface to pilot the experimentation and the results are exported in JSON to be post-processed by our provided tools.

This software is currently under development and can be only used to benchmark software video encoding. In future releases, we will add hardware codecs, benchmark on decoding or video processing...

## Encoding Benchmark

To call the different codecs, we use [FFMPEG](https://ffmpeg.org/) since it provides lot of open-source implementation and it simplifies the usage of hardware codecs.

This benchmark mode takes an original vidéo (which will be re-encoded) and an set of encoder parameters :
- codec and profil used
- preset (only for compatible codec)
- CRF

The parameters can be selected thanks to the GUI interface or directly from the command line:
```
./video-benchmark-tool --cli # for more informations
```

The original video will be re-encoded for each configuration that the user has defined and for each of them we store in JSON result file all these metrics:
- the encoding time
- the size of produced video  (without container)
- the decoding time of re-encoded video
- the [VMAF](https://github.com/Netflix/vmaf) score compared to the original video

This is the structure of JSON file :
```json
{
"experiments": [
	{
		"key":
		{
			"codec_id": integer, // 0: x264 - baseline, 1: x264 - main, 2: x264 - high and 3: x265 - main
			"codec_name": string, // codec pretty name
			"crf": integer, // Integer between 0 and 51
			"preset": string // Name of preset
		},
		"results":
		{
			"bitstream_size": integer, // size in octet
			"decoding_time": integer, // time in ms
			"encoding_time": integer, // time in ms
			"vmaf": double // Minimal VMAF score for all sequence
		}
		// ...
	}
]
}
```

We provide some basic tools to extract useful informations from JSON. See [Tools](#tools) for more details.

## Tools

We provide different tools to exploit results from the benchmark :
- [extract-image-from-video](tools/extract-image-from-video): extract an image form a video
- [extract-pareto-data](tools/extract-pareto-data): generate data file to show pareto fronts (tradeoff between quality and encoding time, quality and size and encoding time and size)
- [glpk-solver](tools/glpk-solver): an linear program to find the optimal solution for specific constraints (obsolete)
- [generate-graph](tools/generate-graph): generate graphs from linaire solver (obsolete)

We provide also some [GNU octave scripts](scripts) to plot pareto fronts from [extract-pareto-data](tools/extract-pareto-data).
