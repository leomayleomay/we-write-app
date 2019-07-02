module CurrentUser = ReasonApollo.CreateQuery(Gql_Schema.CurrentUser);

module Blob = ReasonApollo.CreateQuery(Gql_Schema.Blob);

module Repo = ReasonApollo.CreateQuery(Gql_Schema.Repo);

module RepoFiles = ReasonApollo.CreateQuery(Gql_Schema.RepoFiles);

module MaybeFork = ReasonApollo.CreateQuery(Gql_Schema.MaybeFork);

module MyRepos = ReasonApollo.CreateQuery(Gql_Schema.MyRepos);

module MyRepo = ReasonApollo.CreateQuery(Gql_Schema.MyRepo);

module PullRequests = ReasonApollo.CreateQuery(Gql_Schema.PullRequests);

module PullRequest = ReasonApollo.CreateQuery(Gql_Schema.PullRequest);

module PullRequestReviews =
  ReasonApollo.CreateQuery(Gql_Schema.PullRequestReviews);

module PullRequestComments =
  ReasonApollo.CreateQuery(Gql_Schema.PullRequestComments);