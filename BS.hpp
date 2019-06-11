/**
* Cristina Ruiz Martin
* ARSLab - Carleton University
*
* SenderCadmium:
* Cadmium implementation of CD++ Sender atomic model
*/

/* branch boi */

#ifndef BOOST_SIMULATION_PDEVS_BS_HPP
#define BOOST_SIMULATION_PDEVS_BS_HPP

#include <cadmium/modeling/ports.hpp>
#include <cadmium/modeling/message_bag.hpp>
#include <limits>
#include <math.h> 
#include <assert.h>
#include <memory>
#include <iomanip>
#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include <algorithm>
#include <limits>
#include <random>

#include "../data_structures/message.hpp"

using namespace cadmium;
using namespace std;

//Port definition
    struct BS_defs{
        struct out : public out_port<Message_t> {
        };
        struct in : public in_port<Message_t> {
        };
    };

    template<typename TIME>
    class BS{
        using defs=BS_defs; // putting definitions in context
        public:
            //Parameters to be overwriten when instantiating the atomic model
            TIME   preparationTime;
            TIME   timeout;
            // default constructor
            BS() noexcept{
              preparationTime  = TIME("00:00:10");
              timeout          = TIME("00:00:20");
              state.next_internal    = std::numeric_limits<TIME>::infinity();
              state.model_active     = false;
              state.totalPacketNum = 0;
            }
            
            // state definition
            struct state_type{
              int totalPacketNum;
              bool model_active;
              TIME next_internal;
            }; 
            state_type state;
            // ports definition
            using input_ports=std::tuple<typename defs::in>;
            using output_ports=std::tuple<typename defs::out>;

            // internal transition
            void internal_transition() {
              if (state.model_active){
                state.model_active = false;
              }
              state.next_internal = TIME("inf"); 
            }

            // external transition
            void external_transition(TIME e, typename make_message_bags<input_ports>::type mbs) {
              for(const auto &x : get_messages<typename defs::in>(mbs)){
                state.totalPacketNum = static_cast <int> (x.value);
                cout << "BS has recieved: " << state.totalPacketNum << " from generator" << endl;
                state.model_active = true;  
              }
              state.next_internal = preparationTime;
            }

            // confluence transition
            void confluence_transition(TIME e, typename make_message_bags<input_ports>::type mbs) {
                internal_transition();
                external_transition(TIME(), std::move(mbs));
            }

            // output function
            
            typename make_message_bags<output_ports>::type output() const { 
              typename make_message_bags<output_ports>::type bags;
              Message_t out;
              out.value = state.totalPacketNum;
              if (state.model_active){
                cout << "BS sending: " << state.totalPacketNum << " to Switch" << endl;
                get_messages<typename defs::out>(bags).push_back(out); 
              }           
              return bags;
            }

            // time_advance function
            TIME time_advance() const {
              //cout << "BS has state: " << state.model_active << endl; 
              return state.next_internal;
            }

            friend std::ostringstream& operator<<(std::ostringstream& os, const typename BS<TIME>::state_type& i) {
                os << "totalPacketNum: " << i.totalPacketNum; 
            return os;
            }
        };     


#endif // BOOST_SIMULATION_PDEVS_SENDER_HPP