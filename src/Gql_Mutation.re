module AddStar = ReasonApollo.CreateMutation(Gql_Schema.AddStar);

module RemoveStar = ReasonApollo.CreateMutation(Gql_Schema.RemoveStar);
module CreatePullRequest =
  ReasonApollo.CreateMutation(Gql_Schema.CreatePullRequest);

module RequestReviews =
  ReasonApollo.CreateMutation(Gql_Schema.RequestReviews);

module AddPullRequestReview =
  ReasonApollo.CreateMutation(Gql_Schema.AddPullRequestReview);

module DeletePullRequestReview =
  ReasonApollo.CreateMutation(Gql_Schema.DeletePullRequestReview);

module AddReaction = ReasonApollo.CreateMutation(Gql_Schema.AddReaction);

module RemoveReaction =
  ReasonApollo.CreateMutation(Gql_Schema.RemoveReaction);

module AddComment = ReasonApollo.CreateMutation(Gql_Schema.AddComment);
module DeleteIssueComment =
  ReasonApollo.CreateMutation(Gql_Schema.DeleteIssueComment);

module MergePullRequest =
  ReasonApollo.CreateMutation(Gql_Schema.MergePullRequest);