#include "../../../include/voice/dave/dave.h"
#include "../../../include/log.h"
#include <mlspp/tls/tls_syntax.h>
#include <mlspp/mls/messages.h>


bool Dave::isValidProposal(const mls::ValidatedContent& a){
  if(a.authenticated_content().content.content_type() != mls::ContentType::proposal){
    Log::error("not a proposal");
    return false;
  }
  auto proposal = std::get<mls::Proposal>(a.authenticated_content().content.content);
  // only mls add and remove proposals
  if(proposal.proposal_type() != mls::ProposalType::add && proposal.proposal_type() != mls::ProposalType::remove){
    Log::error("not valid proposal type");
  }
  if(a.authenticated_content().content.sender.sender_type() != mls::SenderType::external){
    Log::error("proposal from a sender that is not the voice gateway");
    return false;
  }
  // TODO: add checking if the member is expected to be in the group
  return true;
}
