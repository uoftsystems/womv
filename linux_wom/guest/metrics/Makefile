all: get_metrics get_segments get_metrics_synthetic get_segments_synthetic

get_metrics: get_metrics.cpp
	g++ --std=c++11 get_metrics.cpp -o get_metrics

get_segments: get_segments.cpp
	g++ --std=c++11 get_segments.cpp -o get_segments

get_metrics_synthetic: get_metrics_synthetic.cpp
	g++ --std=c++11 get_metrics_synthetic.cpp -o get_metrics_synthetic

get_segments_synthetic: get_segments_synthetic.cpp
	g++ --std=c++11 get_segments_synthetic.cpp -o get_segments_synthetic

clean:
	rm -rf get_metrics get_segments get_segments_synthetic get_metrics_synthetic
