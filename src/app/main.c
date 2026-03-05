// src/cli/main.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cleaner/core/config.h"
#include "cleaner/core/core_run.h"
#include "cleaner/platform/platform_api.h"

/* ========================================================= */
/* Printing                                                  */
/* ========================================================= */

static void print_usage(void)
{
  printf("\nCleaner Enterprise\n");
  printf("Usage:\n");
  printf("  cleaner.exe --source <path> [--target <path>] [--dry-run]\n\n");
}

static void print_help(void)
{
  printf("\nCleaner Enterprise v1.0.0\n");
  printf("----------------------------------------\n");
  printf("Usage:\n");
  printf("  cleaner.exe --source <path> [options]\n\n");

  printf("Core Options:\n");
  printf("  --source <path>   Source directory (required)\n");
  printf("  --target <path>   Target directory\n");
  printf("  --dry-run         Simulation only\n");
  printf("  --help            Show this help\n\n");

  printf("Example:\n");
  printf("  cleaner.exe --source C:\\Test --dry-run\n\n");
}

/* ========================================================= */
/* Argument Parsing                                          */
/* ========================================================= */

static int parse_arguments(int argc, char **argv,
                           cleaner_config_t *out_config)
{
  if (!out_config)
    return -1;

  memset(out_config, 0, sizeof(*out_config));

  for (int i = 1; i < argc; ++i)
  {
    if (strcmp(argv[i], "--source") == 0 && i + 1 < argc)
    {
      out_config->source = argv[++i];
    }
    else if (strcmp(argv[i], "--target") == 0 && i + 1 < argc)
    {
      out_config->target = argv[++i];
    }
    else if (strcmp(argv[i], "--dry-run") == 0)
    {
      out_config->dry_run = 1;
    }
    else if (strcmp(argv[i], "--help") == 0)
    {
      print_help();
      exit(0);
    }
    else
    {
      printf("Unknown argument: %s\n", argv[i]);
      return -1;
    }
  }

  if (!out_config->source)
  {
    printf("Error: --source is required.\n");
    return -1;
  }

  return 0;
}

/* ========================================================= */
/* Main                                                      */
/* ========================================================= */

int main(int argc, char **argv)
{
  if (argc < 2)
  {
    print_usage();
    return 1;
  }

  cleaner_config_t config;

  if (parse_arguments(argc, argv, &config) != 0)
  {
    print_usage();
    return 1;
  }

  /* ---------------- Platform API ---------------- */
  const cleaner_platform_api_t *platform =
      cleaner_platform_get_api();

  if (!platform)
  {
    fprintf(stderr, "Platform initialization failed.\n");
    return 1;
  }

  if (platform->abi_version != CLEANER_PLATFORM_ABI_VERSION)
  {
    fprintf(stderr, "Platform ABI mismatch.\n");
    return 1;
  }

  // cleaner_platform_api_t platform;

  // if (cleaner_platform_get_api(&platform) != 0) {
  //   fprintf(stderr, "Platform initialization failed.\n");
  //   return 1;
  // }

  // if (platform.abi_version != CLEANER_PLATFORM_ABI_VERSION)
  // {
  //   fprintf(stderr, "Platform ABI mismatch.\n");
  //   return 1;
  // }

  /* ---------------- Run Core ---------------- */

  // error_t err = core_run(&platform, &config);
  error_t err = core_run(platform, &config);

  if (!error_is_ok(err))
  {
    fprintf(stderr, "Cleaner failed: %s\n",
            err.message ? err.message : "unknown error");
    return 1;
  }

  return 0;
}
