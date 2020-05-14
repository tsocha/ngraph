//*****************************************************************************
// Copyright 2017-2020 Intel Corporation
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//*****************************************************************************

#include "default_opset.hpp"
#include "rnn.hpp"
#include "utils/recurrent.hpp"

namespace ngraph
{
    namespace onnx_import
    {
        namespace op
        {
            namespace set_1
            {
                namespace
                {
                    struct RNNInputMap : public recurrent::OpInputMap
                    {
                        RNNInputMap(const onnx_import::Node& node, std::size_t gates_count)
                            : OpInputMap(node, gates_count)
                        {
                        }

                        virtual ~RNNInputMap() = default;
                    };

                    struct RNNAttributes : public recurrent::OpAttributes
                    {
                        RNNAttributes(const Node& node)
                            : OpAttributes(node)
                        {
                        }

                        virtual ~RNNAttributes() = default;
                    };
                }

                NodeVector rnn(const Node& node)
                {
                    constexpr std::size_t gates_count = 1;
                    RNNInputMap input_map{node, gates_count};
                    RNNAttributes attributes{node};

                    recurrent::RecurrentSequence sequence_op(
                        input_map, attributes, attributes.m_direction);
                    auto results =
                        sequence_op.run_sequence([&attributes](const recurrent::OpInputMap& args,
                                                               const recurrent::OpAttributes& attrs,
                                                               const Output<ngraph::Node>& in_Xt,
                                                               const Output<ngraph::Node> H_t) {

                            const RNNInputMap& rnn_args = dynamic_cast<const RNNInputMap&>(args);
                            const RNNAttributes& rnn_attrs =
                                dynamic_cast<const RNNAttributes&>(attrs);

                            return std::make_shared<default_opset::RNNCell>(
                                in_Xt,
                                H_t,
                                rnn_args.at(recurrent::OpInput::W),
                                rnn_args.at(recurrent::OpInput::R),
                                rnn_args.at(recurrent::OpInput::B),
                                rnn_attrs.m_hidden_size,
                                rnn_attrs.m_activations,
                                rnn_attrs.m_activations_alpha,
                                rnn_attrs.m_activations_beta,
                                rnn_attrs.m_clip_threshold);
                        });
                    return results;
                }
            } // namespace set_1
        }     // namespace op
    }         // namespace onnx_import
} // namespace ngraph
