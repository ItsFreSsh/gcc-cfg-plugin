#include "gcc-plugin.h"
#include "plugin-version.h"

#include "includes/nbsdx/json.hpp"

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

json::JSON output;


namespace {
	const pass_data ssa_pass_data = {
		GIMPLE_PASS,
		"ssa_pass",			/* name */
		OPTGROUP_NONE,		/* optinfo_flags */
		TV_NONE,			/* tv_id */
		PROP_gimple_any,	/* properties_required */
		0,					/* properties_provided */
		0,					/* properties_destroyed */
		0,					/* todo_flags_start */
		0					/* todo_flags_finish */
	};

	struct ssa_pass : gimple_opt_pass {
		ssa_pass(gcc::context *ctx) : gimple_opt_pass(ssa_pass_data, ctx) {}

		virtual unsigned int execute(function *fun) override {
			std::cout << function_name(fun) << std::endl;
			std::cout << "----------" << std::endl;
			basic_block bb;

			FOR_ALL_BB_FN(bb, fun) {
				gimple_bb_info *bb_info = &bb->il.gimple;
				gimple_stmt_iterator gsi;

				// Entry block
				if (bb->index == 0) {
					std::cout << "BLOCK " << bb->index << "(ENTRY)" << std::endl;
				}
				// Exit block
				else if (bb->index == 1) {
					std::cout << "BLOCK " << bb->index << "(EXIT)" << std::endl;
				}
				// Other blocks
				else {
					std::cout << "BLOCK " << bb->index << std::endl << "**" << std::endl;
					for (gsi = gsi_start(bb_info->seq); !gsi_end_p (gsi); gsi_next (&gsi)) {
						gimple* g = gsi_stmt(gsi);
						print_gimple_stmt(stdout, g, 0, (dump_flags_t)0);
						location_t gloc = gimple_lineno(g); // original code line location
						std::cout << "line " << gloc << std::endl;
					}
				}

				edge e;
				edge_iterator ei;

				std::cout << "ROUTES FROM THIS BLOCK: ";
				FOR_EACH_EDGE(e, ei, bb->succs) {
					basic_block dest = e->dest;
					std::cout << dest->index << " ";
				}
				std::cout << std::endl << "***" << std::endl;
			}
			std::cout << "\n\n" << std::endl;
			return 0;
		}
	};
}

// Print json, called after all other passes fnished
void execute_finish_unit (void *gcc_data, void *user_data) {
	std::cout << output << std::endl;
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

	// Register the phase right after ssa
	struct register_pass_info pass_info;

	pass_info.pass = new ssa_pass(g);
	pass_info.reference_pass_name = "ssa";
	pass_info.ref_pass_instance_number = 1;
	pass_info.pos_op = PASS_POS_INSERT_AFTER;
	register_callback(plugin_info->base_name, PLUGIN_PASS_MANAGER_SETUP, NULL, &pass_info);
	register_callback (plugin_info->base_name, PLUGIN_FINISH_UNIT, &execute_finish_unit, NULL);
	return 0;
}