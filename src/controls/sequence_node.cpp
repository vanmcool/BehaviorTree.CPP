/* Copyright (C) 2015-2018 Michele Colledanchise -  All Rights Reserved
 * Copyright (C) 2018-2019 Davide Faconti, Eurecat -  All Rights Reserved
*
*   Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"),
*   to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
*   and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
*   The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*
*   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
*   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
*   WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include "behaviortree_cpp/controls/sequence_node.h"
#include "behaviortree_cpp/action_node.h"

namespace BT
{


SequenceNode::SequenceNode(const std::string& name)
    : ControlNode::ControlNode(name, {} )
{
    setRegistrationID("Sequence");
}

NodeStatus SequenceNode::tick()
{
    const size_t children_count = children_nodes_.size();

    setStatus(NodeStatus::RUNNING);

    for (unsigned int index = 0; index < children_count; index++)
    {
        TreeNode* child_node = children_nodes_[index];

        NodeStatus child_status;

        // special case just for Actions
        if (auto action_child = dynamic_cast<ActionNodeBase*>(child_node) )
        {
            NodeStatus prev_status = action_child->status();
            if (prev_status == NodeStatus::IDLE || prev_status == NodeStatus::RUNNING)
            {
                child_status = action_child->executeTick();
            }
            else{
                child_status = prev_status;
            }
        }
        else{
            child_status = child_node->executeTick();
        }

        switch (child_status)
        {
            case NodeStatus::RUNNING:
            {
                return child_status;
            }
            case NodeStatus::FAILURE:
            {
                haltChildren(0);
                return child_status;
            }
            case NodeStatus::SUCCESS:
            {
                // continue;
            }
            break;

            case NodeStatus::IDLE:
            {
                throw LogicError("This is not supposed to happen");
            }
        }   // end switch
    }       // end for loop

    haltChildren(0);
    return NodeStatus::SUCCESS;
}
}
