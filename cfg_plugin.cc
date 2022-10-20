#include "gcc-plugin.h"
#include "plugin-version.h"

#include "tree-pass.h"
#include "context.h"
#include "basic-block.h"
#include "gimple-pretty-print.h"
#include "tree.h"
#include "gimple-expr.h"
#include "print-tree.h"
#include "gimple.h"
#include "gimple-iterator.h"

#include <iostream>

// We must assert that this plugin is GPL compatible
int plugin_is_GPL_compatible;

// TODO: proper description
static struct plugin_info my_gcc_plugin_info = {"1.0", "A plugin to retrieve cfg of cuntions and output json"};


namespace {
    const pass_data my_first_pass_data = {
        GIMPLE_PASS,
        "my_first_pass", /* name */
        OPTGROUP_NONE,   /* optinfo_flags */
        TV_NONE,         /* tv_id */
        PROP_gimple_any, /* properties_required */
        0,               /* properties_provided */
        0,               /* properties_destroyed */
        0,               /* todo_flags_start */
        0                /* todo_flags_finish */
    };

    struct my_first_pass : gimple_opt_pass {
        my_first_pass(gcc::context *ctx) : gimple_opt_pass(my_first_pass_data, ctx) {
        }

        virtual unsigned int execute(function *fun) override {
        basic_block bb;
        gimple_stmt_iterator gsi;

        std::cerr << "subgraph fun_" << fun << " {\n";

        FOR_EACH_BB_FN(bb, fun) {
        gimple_bb_info *bb_info = &bb->il.gimple;
        //std::cerr << "bb_" << fun << "_" << bb->index << "[label=\"";

        // Entry block
        if (bb->index == 0) {
          std::cout << "ENTERING function: " << function_name(fun) << " in file: " << (LOCATION_FILE(fun->function_start_locus) ?: "<unknown>")
                    << " on line:" << LOCATION_LINE(fun->function_start_locus) << std::endl;
        }
        // Exit block
        else if (bb->index == 1) {
          std::cout << "EXITING function: " << function_name(fun) << " in file: " << (LOCATION_FILE(fun->function_end_locus) ?: "<unknown>") << " on line :" << LOCATION_LINE(fun->function_end_locus) << std::endl;
        }
        // Other blocks
        else {
          std::cout << "block " << bb->index << std::endl;
          print_gimple_seq(stderr, bb_info->seq, 0, (dump_flags_t)0);
          print_gimple_seq(stdout, bb_info->seq, 0, (dump_flags_t)0);
          // gimple_stmt_iterator gsi;

          // for (gsi = gsi_start (bb_info->seq); !gsi_end_p (gsi); gsi_next (&gsi))
          //{
          //     gimple* g = gsi_stmt(gsi);
          /* Do something with gimple statement G.  */
          // std::cout << "--" << g->code << "--" << std::endl;
          //}
          // std::cout << "line" << gimple_seq_first() << std::endl;
          std::cout << "----" << std::endl;
          // std::cout << BB_HEAD << std::endl;
        }

        edge e;
        edge_iterator ei;

        FOR_EACH_EDGE(e, ei, bb->succs)
        {
          basic_block dest = e->dest;
          std::cerr << "bb_" << fun << "_" << bb->index << " -> bb_" << fun << "_" << dest->index << ";\n";
          std::cout << function_name(fun) << " block " << bb->index << " -> " << dest->index << std::endl;
          // fun->gimple_body
        }
      }
      return 0;
    }
  };
}


// Plugin initialization
int plugin_init(struct plugin_name_args *plugin_info, struct plugin_gcc_version *version) {
    // Check plugin version against gcc version
    if (!plugin_default_version_check(version, &gcc_version)) {
        std::cerr << "This GCC plugin is for version " << GCCPLUGIN_VERSION_MAJOR
            << "." << GCCPLUGIN_VERSION_MINOR << "\n";
    return 1;
}

// Register callback function
register_callback(plugin_info->base_name,
                    PLUGIN_INFO,            /* event */
                    NULL,                   /* callback */
                    &my_gcc_plugin_info);   /* user_data */

  // Register the phase right after cfg
  struct register_pass_info pass_info;

  pass_info.pass = new my_first_pass(g);
  pass_info.reference_pass_name = "cfg";
  pass_info.ref_pass_instance_number = 1;
  pass_info.pos_op = PASS_POS_INSERT_AFTER;

  register_callback(plugin_info->base_name, PLUGIN_PASS_MANAGER_SETUP, NULL, &pass_info);

  return 0;
}