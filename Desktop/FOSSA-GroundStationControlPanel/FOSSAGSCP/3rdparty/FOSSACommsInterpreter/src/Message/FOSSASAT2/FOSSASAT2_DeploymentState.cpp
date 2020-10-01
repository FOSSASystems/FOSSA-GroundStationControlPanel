// MIT LICENSE
//
// Copyright (c) 2020 FOSSA Systems
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the
// "Software"), to deal in the Software without restriction, including
// without limitation the rights to use, copy, modify, merge, publish,
// distribute, sublicense, and/or sell copies of the Software, and to
// permit persons to whom the Software is furnished to do so, subject to
// the following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
// CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
// TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
// SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//

#include "FOSSASAT2_DeploymentState.h"

FOSSASAT2::Messages::DeploymentState::DeploymentState(Frame &frame)
{
    this->deploymentCounter = frame.GetByteAt(0);
}

std::string FOSSASAT2::Messages::DeploymentState::ToString()
{
    std::stringstream ss;
    ss << "Satellite Version: FOSSASAT2" << std::endl;
    ss << "Message Name: DeploymentState" << std::endl;
    ss << "Deployment counter: " << this->deploymentCounter << std::endl;

    std::string out;
    ss >> out;
    return out;
}

std::string FOSSASAT2::Messages::DeploymentState::ToJSON()
{
    std::stringstream ss;
    ss << "{" << std::endl;
    ss << "\"Satellite Version\": \"FOSSASAT2\"," << std::endl;
    ss << "\"Message Name\": \"DeploymentState\"," << std::endl;
    ss << "\"Deployment counter\": " << this->deploymentCounter << std::endl;
    ss << "}" << std::endl;

    std::string out;
    ss >> out;
    return out;
}

uint8_t FOSSASAT2::Messages::DeploymentState::GetDeploymentCounter() const
{
    return deploymentCounter;
}
