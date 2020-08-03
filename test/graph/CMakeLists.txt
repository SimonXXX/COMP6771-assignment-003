cxx_test(
   TARGET graph_test1
   FILENAME "graph_test1.cpp"
   LINK fmt::fmt-header-only range-v3 Catch2::Catch2
)
cxx_test(
   TARGET graph_test2
   FILENAME "graph_test2.cpp"
   LINK absl::flat_hash_set absl::flat_hash_map gsl::gsl-lite-v1 fmt::fmt-header-only range-v3
)
cxx_test(
   TARGET graph_test3
   FILENAME "graph_test3.cpp"
   LINK absl::flat_hash_set absl::flat_hash_map gsl::gsl-lite-v1 fmt::fmt-header-only range-v3
)
cxx_test(
   TARGET graph_test4
   FILENAME "graph_test4.cpp"
   LINK absl::flat_hash_set absl::flat_hash_map gsl::gsl-lite-v1 fmt::fmt-header-only range-v3
)
cxx_test(
   TARGET graph_test5
   FILENAME "graph_test5.cpp"
   LINK absl::flat_hash_set absl::flat_hash_map gsl::gsl-lite-v1 fmt::fmt-header-only range-v3
)
