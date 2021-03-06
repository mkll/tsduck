//----------------------------------------------------------------------------
//
// TSDuck - The MPEG Transport Stream Toolkit
// Copyright (c) 2005-2020, Thierry Lelegard
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice,
//    this list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
// THE POSSIBILITY OF SUCH DAMAGE.
//
//----------------------------------------------------------------------------

#include "tsECMRepetitionRateDescriptor.h"
#include "tsDescriptor.h"
#include "tsTablesDisplay.h"
#include "tsPSIRepository.h"
#include "tsDuckContext.h"
#include "tsNames.h"
#include "tsxmlElement.h"
TSDUCK_SOURCE;

#define MY_XML_NAME u"ECM_repetition_rate_descriptor"
#define MY_CLASS ts::ECMRepetitionRateDescriptor
#define MY_DID ts::DID_ECM_REPETITION_RATE
#define MY_STD ts::STD_DVB

TS_REGISTER_DESCRIPTOR(MY_CLASS, ts::EDID::Standard(MY_DID), MY_XML_NAME, MY_CLASS::DisplayDescriptor);


//----------------------------------------------------------------------------
// Constructor.
//----------------------------------------------------------------------------

ts::ECMRepetitionRateDescriptor::ECMRepetitionRateDescriptor() :
    AbstractDescriptor(MY_DID, MY_XML_NAME, MY_STD, 0),
    CA_system_id(0),
    ECM_repetition_rate(0),
    private_data()
{
    _is_valid = true;
}

ts::ECMRepetitionRateDescriptor::ECMRepetitionRateDescriptor(DuckContext& duck, const Descriptor& desc) :
    ECMRepetitionRateDescriptor()
{
    deserialize(duck, desc);
}


//----------------------------------------------------------------------------
// Serialization
//----------------------------------------------------------------------------

void ts::ECMRepetitionRateDescriptor::serialize(DuckContext& duck, Descriptor& desc) const
{
    ByteBlockPtr bbp(serializeStart());
    bbp->appendUInt16(CA_system_id);
    bbp->appendUInt16(ECM_repetition_rate);
    bbp->append(private_data);
    serializeEnd(desc, bbp);
}


//----------------------------------------------------------------------------
// Deserialization
//----------------------------------------------------------------------------

void ts::ECMRepetitionRateDescriptor::deserialize(DuckContext& duck, const Descriptor& desc)
{
    private_data.clear();

    const uint8_t* data = desc.payload();
    size_t size = desc.payloadSize();

    _is_valid = desc.isValid() && desc.tag() == _tag && size >= 4;

    if (_is_valid) {
        CA_system_id = GetUInt16(data);
        ECM_repetition_rate = GetUInt16(data + 2);
        private_data.copy(data + 4, size - 4);
    }
}


//----------------------------------------------------------------------------
// XML serialization
//----------------------------------------------------------------------------

void ts::ECMRepetitionRateDescriptor::buildXML(DuckContext& duck, xml::Element* root) const
{
    root->setIntAttribute(u"CA_system_id", CA_system_id, true);
    root->setIntAttribute(u"ECM_repetition_rate", ECM_repetition_rate, false);
    if (!private_data.empty()) {
        root->addElement(u"private_data")->addHexaText(private_data);
    }
}


//----------------------------------------------------------------------------
// XML deserialization
//----------------------------------------------------------------------------

void ts::ECMRepetitionRateDescriptor::fromXML(DuckContext& duck, const xml::Element* element)
{
    private_data.clear();

    _is_valid =
        checkXMLName(element) &&
        element->getIntAttribute<uint16_t>(CA_system_id, u"CA_system_id", true) &&
        element->getIntAttribute<uint16_t>(ECM_repetition_rate, u"ECM_repetition_rate", true) &&
        element->getHexaTextChild(private_data, u"private_data", false, 0, MAX_DESCRIPTOR_SIZE - 6);
}


//----------------------------------------------------------------------------
// Static method to display a descriptor.
//----------------------------------------------------------------------------

void ts::ECMRepetitionRateDescriptor::DisplayDescriptor(TablesDisplay& display, DID did, const uint8_t* data, size_t size, int indent, TID tid, PDS pds)
{
    DuckContext& duck(display.duck());
    std::ostream& strm(duck.out());
    const std::string margin(indent, ' ');

    if (size >= 4) {
        strm << margin << UString::Format(u"CA System Id: %s", { names::CASId(duck, GetUInt16(data), names::FIRST) }) << std::endl
             << margin << UString::Format(u"ECM repetition rate: %d ms", { GetUInt16(data + 2) }) << std::endl;
        display.displayPrivateData(u"Private data", data + 4, size - 4, indent);
    }
    else {
        display.displayExtraData(data, size, indent);
    }
}
