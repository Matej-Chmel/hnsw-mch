#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "benchmark_runner.h"
using namespace mch;
namespace py = pybind11;

PYBIND11_MODULE(hnsw_mch, package)
{
	package.doc() = "A Hiearchical Navigable Small World implementation.";

	py::class_<Config>(package, "Config")
		.def(py::init<size_t, bool, bool, size_t, float, size_t, bool>(),
			py::arg("ef"), py::arg("extend_candidates"), py::arg("keep_pruned"), py::arg("m"),
			py::arg("ml"), py::arg("mmax"), py::arg("use_heuristic")
		)
		.def_readonly("ef", &Config::ef)
		.def_readonly("extend_candidates", &Config::extend_candidates)
		.def_readonly("keep_pruned", &Config::keep_pruned)
		.def_readonly("m", &Config::m)
		.def_readonly("ml", &Config::ml)
		.def_readonly("mmax", &Config::mmax)
		.def_readonly("use_heuristic", &Config::use_heuristic)
		.def("__str__", &Config::to_string);
	
	py::class_<Benchmark>(package, "Benchmark")
		.def(py::init<Config*>(), py::arg("config"))
		.def_readonly("build_time", &Benchmark::build_time)
		.def_readonly("config", &Benchmark::config)
		.def_readonly("recall", &Benchmark::recall)
		.def_readonly("search_times", &Benchmark::search_times)
		.def("run", &Benchmark::run, py::arg("bruteforce"), py::arg("nodes"), py::arg("queries"), py::arg("ef"), py::arg("k"));

	py::class_<BenchmarkRunner>(package, "BenchmarkRunner")
		.def(py::init<size_t, size_t, size_t, float, float, size_t>(),
			py::arg("dimensions"), py::arg("node_count"), py::arg("query_count"), py::arg("min_value"), py::arg("max_value"), py::arg("k")
		)
		.def(py::init<size_t, const char*, const char*, const char*, size_t>(),
			py::arg("dimensions"), py::arg("node_path"), py::arg("query_path"), py::arg("bruteforce_path"), py::arg("k")
		)
		.def_readonly("bruteforce_time", &BenchmarkRunner::bruteforce_time)
		.def_readonly("benchmarks", &BenchmarkRunner::benchmarks)
		.def("add", &BenchmarkRunner::add, py::arg("config"), py::arg("ef"))
		.def("reserve", &BenchmarkRunner::reserve, py::arg("capacity"));
}
