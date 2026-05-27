// SPDX-License-Identifier: CDDL-1.0
/*
 * CDDL HEADER START
 *
 * The contents of this file are subject to the terms of the
 * Common Development and Distribution License (the "License").
 * You may not use this file except in compliance with the License.
 *
 * You can obtain a copy of the license at usr/src/OPENSOLARIS.LICENSE
 * or https://opensource.org/licenses/CDDL-1.0.
 * See the License for the specific language governing permissions
 * and limitations under the License.
 *
 * When distributing Covered Code, include this CDDL HEADER in each
 * file and include the License file at usr/src/OPENSOLARIS.LICENSE.
 * If applicable, add the following below this CDDL HEADER, with the
 * fields enclosed by brackets "[]" replaced with your own identifying
 * information: Portions Copyright [yyyy] [name of copyright owner]
 *
 * CDDL HEADER END
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <filesystem>
#include <format>

void usage(const char* exec_name)
{
  std::string output = std::format("{} Usage: access <file path>", exec_name);
  fprintf(stderr, "%s\n", output.c_str());
  exit(-2);
}

int main(int argc, const char* argv[]) {
  if (argc != 2 || argv[1] == NULL || *argv[1] == 0) {
    usage(argv[0]);
  }
  std::error_code ec;
  auto Stat = std::filesystem::symlink_status(argv[1], ec);
  if (!ec) {
    std::string output;
    if (std::filesystem::is_regular_file(Stat))
      output = std::format("READ Permission OK on {}", argv[1]);
    else if (std::filesystem::is_directory(Stat))
      output = std::format("READ Permission OK on {}", argv[1]);
    else if (std::filesystem::is_symlink(Stat))
      output = std::format("LREAD Permission OK on {}", argv[1]);
    else if (std::filesystem::exists(Stat))
      output = std::format("EREAD Permission OK on {}", argv[1]);
    else
      output = std::format("Not available {}", argv[1]);
    fprintf(stdout, "%s\n", output.c_str());
    fflush(stdout);
    return 0;
  }
  std::string output = std::format("{} {}", ec.message(), argv[1]);
  fprintf(stderr, "%s\n", output.c_str());
  return -1;
}
