// Copyright (c) by respective owners including Yahoo!, Microsoft, and
// individual contributors. All rights reserved. Released under a BSD (revised)
// license as described in the file LICENSE.

#include <cstring>
#include <cfloat>
#include <cmath>
#include <cstdio>

#include "cache.h"
#include "accumulate.h"
#include "best_constant.h"
#include "vw_string_view.h"
#include "example.h"
#include "vw_string_view_fmt.h"

#include "io/logger.h"
// needed for printing ranges of objects (eg: all elements of a vector)
#include <fmt/ranges.h>

namespace logger = VW::io::logger;

namespace no_label
{
void parse_no_label(const std::vector<VW::string_view>& words)
{
  switch (words.size())
  {
    case 0:
      break;
    default:
      logger::log_error("Error: {0} is too many tokens for a simple label: {1}",
			words.size(), fmt::join(words, " "));
  }
}

// clang-format off
label_parser no_label_parser = {
  // default_label
  [](polylabel*) {},
  // parse_label
  [](parser*, shared_data*, polylabel*, std::vector<VW::string_view>& words, reduction_features&) {
    parse_no_label(words);
  },
  // cache_label
  [](polylabel*, reduction_features&, io_buf&) {},
  // read_cached_label
  [](shared_data*, polylabel*, reduction_features&, io_buf&) -> size_t { return 1; },
   // get_weight
  [](polylabel*, const reduction_features&) { return 1.f; },
  // test_label
  [](polylabel*) { return false; },
  label_type_t::nolabel
};
// clang-format on

void print_no_label_update(vw& all, example& ec)
{
  if (all.sd->weighted_labeled_examples + all.sd->weighted_unlabeled_examples >= all.sd->dump_interval &&
      !all.logger.quiet && !all.bfgs)
  {
    all.sd->print_update(*all.trace_message, all.holdout_set_off, all.current_pass, 0.f, ec.pred.scalar,
        ec.num_features, all.progress_add, all.progress_arg);
  }
}

void output_and_account_no_label_example(vw& all, example& ec)
{
  all.sd->update(ec.test_only, false, ec.loss, ec.weight, ec.num_features);

  all.print_by_ref(all.raw_prediction.get(), ec.partial_prediction, -1, ec.tag);
  for (auto& sink : all.final_prediction_sink) { all.print_by_ref(sink.get(), ec.pred.scalar, 0, ec.tag); }

  print_no_label_update(all, ec);
}

void return_no_label_example(vw& all, void*, example& ec)
{
  output_and_account_example(all, ec);
  VW::finish_example(all, ec);
}
}  // namespace no_label
