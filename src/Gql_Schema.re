module CurrentUser = [%graphql
  {|
    query {
      viewer {
        name
        email
        login
        avatarUrl
      }
    }
  |}
];

module Blob = [%graphql
  {|
    query blob($owner: String!, $name: String!, $expression: String!) {
      repository(owner: $owner, name: $name) {
        id
        name
        description
        nameWithOwner
        viewerHasStarred
        viewerPermission
        isPrivate
        stargazers {
          totalCount
        }
        forks {
          totalCount
        }
        obj: object(expression: $expression) {
          ... on Blob {
            oid
            text
          }
        }
      }
    }
  |}
];

module MaybeFork = [%graphql
  {|
    query ($owner: String!, $name: String!,  $qualifiedName: String!) {
      repository(owner: $owner, name: $name) {
        id
        viewerPermission
        ref(qualifiedName: $qualifiedName) {
          target {
            oid
          }
        }
      }
    }
  |}
];

module Repo = [%graphql
  {|
    query repo($owner: String!, $name: String!) {
      repository(owner: $owner, name: $name) {
        id
        name
        description
        nameWithOwner
        viewerHasStarred
        viewerPermission
        isPrivate
        stargazers {
          totalCount
        }
        forks {
          totalCount
        }
      }
    }
  |}
];

module RepoFiles = [%graphql
  {|
    query repo($owner: String!, $name: String!) {
      repository(owner: $owner, name: $name) {
        id
        name
        description
        nameWithOwner
        viewerHasStarred
        viewerPermission
        isPrivate
        stargazers {
          totalCount
        }
        forks {
          totalCount
        }
        obj: object(expression: "master:") {
          ... on Tree {
            entries {
              name
              type_: type
            }
          }
        }
      }
    }
  |}
];

module PullRequests = [%graphql
  {|
    query ($owner:String!, $name: String!, $first: Int, $last: Int, $before: String, $after: String) {
      repository(owner: $owner, name: $name) {
        id
        name
        description
        nameWithOwner
        viewerHasStarred
        viewerPermission
        stargazers {
          totalCount
        }
        forks {
          totalCount
        }
        pullRequests(first: $first, last: $last, before: $before, after: $after, states: [OPEN], orderBy: {field: UPDATED_AT, direction: DESC}) {
          nodes {
            id
            number
            title
          }

          pageInfo {
            hasNextPage
            hasPreviousPage
            endCursor
            startCursor
          }
        }
      }
    }
  |}
];

module PullRequest = [%graphql
  {|
    query ($owner:String!, $name: String!, $number: Int!) {
      repository(owner: $owner, name: $name) {
        id
        name
        description
        nameWithOwner
        viewerHasStarred
        viewerPermission
        stargazers {
          totalCount
        }
        forks {
          totalCount
        }
        pullRequest(number: $number) {
          id
          number
          title
          bodyText
          viewerCanReact
          mergeable
          state
          viewerDidAuthor
          createdAt
          approvals: reviews(states: APPROVED) {
            totalCount
          }
          reactionGroups {
            content
            users {
              totalCount
            }
            viewerHasReacted
          }
          author {
            login
            avatarUrl
          }
        }
      }
    }
  |}
];

module PullRequestReviews = [%graphql
  {|
    query pullRequestReviews($owner:String!, $name: String!, $number: Int!, $first: Int, $last: Int, $before: String, $after: String) {
      repository(owner: $owner, name: $name) {
        id
        pullRequest(number: $number) {
          id
          reviews(first: $first, last: $last, before: $before, after: $after) {
            nodes {
              id
              state
              author {
                login
                avatarUrl
              }
              bodyText
              viewerCanReact
              viewerCanDelete
              createdAt
              reactionGroups {
                content
                users {
                  totalCount
                }
                viewerHasReacted
              }
            }

            pageInfo {
              hasNextPage
              hasPreviousPage
              endCursor
              startCursor
            }
          }
        }
      }
    }
  |}
];

module PullRequestComments = [%graphql
  {|
    query pullRequestComments($owner:String!, $name: String!, $number: Int!, $first: Int, $last: Int, $before: String, $after: String) {
      repository(owner: $owner, name: $name) {
        id
        pullRequest(number: $number) {
          id
          bodyText
          createdAt
          viewerCanReact
          author {
            login
            avatarUrl
          }
          reactionGroups {
            content
            users {
              totalCount
            }
            viewerHasReacted
          }
          comments(first: $first, last: $last, before: $before, after: $after) {
            nodes {
              id
              author {
                login
                avatarUrl
              }
              bodyText
              viewerCanReact
              viewerCanDelete
              createdAt
              reactionGroups {
                content
                users {
                  totalCount
                }
                viewerHasReacted
              }
            }

            pageInfo {
              hasNextPage
              hasPreviousPage
              endCursor
              startCursor
            }
          }
        }
      }
    }
  |}
];

module MyRepos = [%graphql
  {|
    query ($first: Int, $last: Int, $before: String, $after: String) {
      viewer {
        repositories(first: $first, last: $last, before: $before, after: $after, orderBy: {field: UPDATED_AT, direction: DESC}) {
          nodes {
            id
            name
            description
            nameWithOwner
            isPrivate
            stargazers {
              totalCount
            }
            forks {
              totalCount
            }
          }

          pageInfo {
            hasNextPage
            hasPreviousPage
            endCursor
            startCursor
          }
        }
      }
    }
  |}
];

module MyRepo = [%graphql
  {|
    query ($name: String!) {
      viewer {
        repository(name: $name) {
          id
          name
          description
        }
      }
    }
  |}
];

module AddStar = [%graphql
  {|
    mutation addStar($starrableId: ID!) {
      addStar(input: {starrableId: $starrableId}) {
        starrable {
          id
        }
      }
    }
  |}
];

module RemoveStar = [%graphql
  {|
    mutation removeStar($starrableId: ID!) {
      removeStar(input: {starrableId: $starrableId}) {
        starrable {
          id
        }
      }
    }
  |}
];

module CreatePullRequest = [%graphql
  {|
    mutation createPullRequest($repositoryId: ID!, $baseRefName: String!, $headRefName: String!, $title: String!, $body: String) {
      createPullRequest(input: {repositoryId: $repositoryId, baseRefName: $baseRefName, headRefName: $headRefName, title: $title, body: $body}) {
        pullRequest {
          number
        }
      }
    }
  |}
];

module RequestReviews = [%graphql
  {|
    mutation requestReviews($pullRequestId: ID!, $userIds: [ID!]) {
      requestReviews(input: {pullRequestId: $pullRequestId, userIds: $userIds}) {
        pullRequest {
          id
        }
      }
    }
  |}
];

module AddReaction = [%graphql
  {|
    mutation addReaction($subjectId: ID!, $content: ReactionContent!) {
      addReaction(input: {subjectId: $subjectId, content: $content}) {
        reaction {
          id
          content
          createdAt
        }
      }
    }
  |}
];

module RemoveReaction = [%graphql
  {|
    mutation removeReaction($subjectId: ID!, $content: ReactionContent!) {
      removeReaction(input: {subjectId: $subjectId, content: $content}) {
        reaction {
          id
          content
          createdAt
        }
      }
    }
  |}
];

module AddPullRequestReview = [%graphql
  {|
    mutation addPullRequestReview($pullRequestId: ID!, $body: String, $event: PullRequestReviewEvent) {
      addPullRequestReview(input: {pullRequestId: $pullRequestId, body: $body, event: $event}) {
        pullRequestReview {
          id
        }
      }
    }
  |}
];

module DeletePullRequestReview = [%graphql
  {|
    mutation deletePullRequestReview($pullRequestReviewId: ID!) {
      deletePullRequestReview(input: {pullRequestReviewId: $pullRequestReviewId}) {
        pullRequestReview {
          id
        }
      }
    }
  |}
];

module AddComment = [%graphql
  {|
    mutation addComment($subjectId: ID!, $body: String!) {
      addComment(input: {subjectId: $subjectId, body: $body}) {
        subject {
          id
        }
      }
    }
  |}
];

module DeleteIssueComment = [%graphql
  {|
    mutation deleteIssueComment($id: ID!) {
      deleteIssueComment(input: {id: $id}) {
        clientMutationId
      }
    }
  |}
];

module MergePullRequest = [%graphql
  {|
    mutation mergePullRequest($pullRequestId: ID!) {
      mergePullRequest(input: {pullRequestId: $pullRequestId}) {
        pullRequest {
          id
        }
      }
    }
  |}
];