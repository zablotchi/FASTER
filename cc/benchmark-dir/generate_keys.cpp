// Quick synthetic key generator for FASTER benchmark testing
// Usage: ./generate_keys <output_file> <num_keys>

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <random>
#include <string>

int main(int argc, char* argv[]) {
  if (argc != 3) {
    fprintf(stderr, "Usage: %s <output_file> <num_keys>\n", argv[0]);
    fprintf(stderr, "Example: %s load.dat 10000000\n", argv[0]);
    exit(1);
  }

  std::string output_file = argv[1];
  uint64_t num_keys = std::stoull(argv[2]);

  printf("Generating %lu keys to %s...\n", num_keys, output_file.c_str());

  FILE* fp = fopen(output_file.c_str(), "wb");
  if (!fp) {
    fprintf(stderr, "Failed to open output file\n");
    exit(1);
  }

  // Use fixed seed for reproducibility
  std::mt19937_64 rng(42);
  std::uniform_int_distribution<uint64_t> dist(0, 268435456);  // kMaxKey

  const uint64_t chunk_size = 1000000;  // Write 1M keys at a time
  uint64_t* buffer = new uint64_t[chunk_size];

  for (uint64_t i = 0; i < num_keys; ) {
    uint64_t batch = std::min(chunk_size, num_keys - i);

    // Generate keys
    for (uint64_t j = 0; j < batch; ++j) {
      buffer[j] = dist(rng);
    }

    // Write to file
    size_t written = fwrite(buffer, sizeof(uint64_t), batch, fp);
    if (written != batch) {
      fprintf(stderr, "Write failed at offset %lu\n", i);
      exit(1);
    }

    i += batch;
    if (i % 10000000 == 0) {
      printf("  Generated %lu / %lu keys (%.1f%%)\n",
             i, num_keys, (double)i * 100.0 / num_keys);
    }
  }

  delete[] buffer;
  fclose(fp);

  printf("Done! Generated %lu keys (%lu MB)\n",
         num_keys, (num_keys * sizeof(uint64_t)) / (1024 * 1024));

  return 0;
}
