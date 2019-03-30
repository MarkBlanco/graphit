//
// Created by Yunming Zhang on 3/20/19.
//

#include <graphit/midend/priority_features_lowering.h>

namespace graphit {

    void PriorityFeaturesLower::lower() {

        std::vector<mir::FuncDecl::Ptr> functions = mir_context_->getFunctionList();

        // find the schedules specified for priority updates,
        // assuming we only have one priority queue at the moment
        auto schedule_finder = PriorityUpdateScheduleFinder(mir_context_, schedule_);

        //this visitor sets the priorty update type, and delta in mir_context
        for (auto function : functions) {
            function->accept(&schedule_finder);
        }

        // lowers the Priority Queue Type, and Alloc Expr based on the schedule
        auto lower_priority_queue_type_and_alloc_expr = LowerPriorityQueueTypeandAllocExpr(mir_context_, schedule_);

        for (auto constant : mir_context_->getConstants()) {
            constant->accept(&lower_priority_queue_type_and_alloc_expr);
        }
        for (auto function : functions) {
            function->accept(&lower_priority_queue_type_and_alloc_expr);
        }

        // Detect pattern for OrderedProcessingOperator, and lower into the MIR node for OrderedProcessingOp
        auto lower_ordered_processing_op = LowerIntoOrderedProcessingOperatorRewriter();
        for (auto function : functions) {
            lower_ordered_processing_op.rewrite(function);
        }

        // lowers the extern apply expression
        auto lower_extern_apply_expr = LowerUpdatePriorityExternVertexSetApplyExpr(schedule_, mir_context_);
        for (auto function : functions) {
            lower_extern_apply_expr.rewrite(function);
        }

    }

    void PriorityFeaturesLower::PriorityUpdateScheduleFinder::visit(
            mir::UpdatePriorityEdgeSetApplyExpr::Ptr update_priority_edgeset_apply_expr) {
        if (schedule_ != nullptr && schedule_->apply_schedules != nullptr) {
            auto current_label = label_scope_.getCurrentScope();
            setPrioritySchedule(current_label);
        }
    }

    void PriorityFeaturesLower::PriorityUpdateScheduleFinder::visit(
            mir::UpdatePriorityExternVertexSetApplyExpr::Ptr update_priority_extern_vertexset_apply_expr) {
        if (schedule_ != nullptr && schedule_->apply_schedules != nullptr) {
            auto current_label = label_scope_.getCurrentScope();
            setPrioritySchedule(current_label);
        }
    }

    void PriorityFeaturesLower::PriorityUpdateScheduleFinder::setPrioritySchedule(std::string current_label) {
        auto apply_schedule = schedule_->apply_schedules->find(current_label);
        if (apply_schedule != schedule_->apply_schedules->end()) { //a schedule is found
            if (apply_schedule->second.priority_update_type
                == ApplySchedule::PriorityUpdateType::REDUCTION_BEFORE_UPDATE) {
                mir_context_->priority_update_type = mir::PriorityUpdateType::ReduceBeforePriorityUpdate;
            } else if (apply_schedule->second.priority_update_type
                       == ApplySchedule::PriorityUpdateType::EAGER_PRIORITY_UPDATE) {
                mir_context_->priority_update_type = mir::PriorityUpdateType::EagerPriorityUpdate;
            } else if (apply_schedule->second.priority_update_type
                       == ApplySchedule::PriorityUpdateType::CONST_SUM_REDUCTION_BEFORE_UPDATE) {
                mir_context_->priority_update_type = mir::PriorityUpdateType::ConstSumReduceBeforePriorityUpdate;
            } else if (apply_schedule->second.priority_update_type
                       == ApplySchedule::PriorityUpdateType::EAGER_PRIORITY_UPDATE_WITH_MERGE) {
                mir_context_->priority_update_type = mir::PriorityUpdateType::EagerPriorityUpdateWithMerge;
            } else {
                mir_context_->priority_update_type = mir::PriorityUpdateType::NoPriorityUpdate;
            }

            if (apply_schedule->second.delta > 1) {
                mir_context_->delta_ = apply_schedule->second.delta;
            }

        } else {
            mir_context_->priority_update_type = mir::PriorityUpdateType::NoPriorityUpdate;
        }

    }

    void PriorityFeaturesLower::LowerUpdatePriorityExternVertexSetApplyExpr::visit(mir::ExprStmt::Ptr expr_stmt) {
        MIRRewriter::visit(expr_stmt);
        if (mir::isa<mir::UpdatePriorityExternVertexSetApplyExpr>(expr_stmt->expr)) {

/*
		mir::UpdatePriorityExternCall::Ptr call_stmt = make_shared<mir::UpdatePriorityExternCall>();
		call_stmt->input_set = expr_stmt->target;
		call_stmt->apply_function_name = expr_stmt->apply_function_name;
		call_stmt->lambda_name = mir_context_->getUniqueNameCounterString();
		call_stmt->output_set_name = mir_context_->getUniqueNameCounterString();	
		
			
		mir::UpdatePriorityUpdateBucketsCall::Ptr update_call = make_shared<mir::UpdatePriorityUpdateBucketsCall>();
		update_call->lambda_name = call_stmt->lambda_name;
		call_stmt->modified_vertexsubset_name = call_stmt->output_set_name;
		
		mir::StmtBlock::Ptr stmt_block = make_shared<mir::StmtBlock>();
*/
        }
        node = expr_stmt;
    }

    void PriorityFeaturesLower::LowerIntoOrderedProcessingOperatorRewriter::visit(mir::WhileStmt::Ptr while_stmt) {
        // check if it matches the pattern
//        if (checkWhileStmtPattern(while_stmt)) {
//            // if matches the pattern, then replace with a separate operator
//            mir::OrderedProcessingOperator::Ptr ordered_op = std::make_shared<mir::OrderedProcessingOperator>();
//            node = ordered_op;
//        } else {
//            node = while_stmt;
//        }


        //mir::OrderedProcessingOperator::Ptr ordered_op = std::make_shared<mir::OrderedProcessingOperator>();
        //node = ordered_op;
        node = while_stmt;
    }


    // One example of a pattern that we are detecting
    //"  while (pq.finished() == false) "
    //"    var frontier : vertexsubset = pq.dequeue_lowest_priority(); % dequeue_ready_set() \n"
    //"    #s1# edges.from(frontier).applyUpdatePriority(updateEdge);  \n"
    //"    delete frontier; "

    bool PriorityFeaturesLower::LowerIntoOrderedProcessingOperatorRewriter::checkWhileStmtPattern(
            mir::WhileStmt::Ptr while_stmt) {
        auto stmt_blk = while_stmt->body;
        int num_stmts = (*(stmt_blk->stmts)).size();

        // for now assuming
        if (num_stmts < 2 || num_stmts > 3) {
            return false;
        }

        mir::Stmt::Ptr first_stmt = (*(stmt_blk->stmts))[0];
        mir::Stmt::Ptr second_stmt = (*(stmt_blk->stmts))[1];


        if (mir::isa<mir::VarDecl>(first_stmt)) {
            // first line should be var decl
            mir::VarDecl::Ptr frontier_var_decl = mir::to<mir::VarDecl>(first_stmt);

            // second line should be expr stmt
            if (mir::isa<mir::ExprStmt>(second_stmt)) {
                mir::ExprStmt::Ptr expr_stmt = mir::to<mir::ExprStmt>(second_stmt);
                if (mir::isa<mir::UpdatePriorityEdgeSetApplyExpr>(expr_stmt->expr)) {
                    return true;
                }
            }
        }
        return false;
    }
}