cxx_executable(
   TARGET "client1"
   FILENAME "client1.cpp"
   LINK absl::flat_hash_set absl::flat_hash_map gsl::gsl-lite-v1 fmt::fmt-header-only range-v3
)
