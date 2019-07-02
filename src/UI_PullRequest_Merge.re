let pullRequestId = (data: Js.t('a)) => {
  Belt.Option.(
    data##repository
    ->flatMap(repo => repo##pullRequest)
    ->map(pr => pr##id)
    ->getExn
  );
};

let viewerPermission = (data: Js.t('a)) => {
  Belt.Option.(
    data##repository
    ->flatMap(repo => repo##viewerPermission)
    ->getWithDefault(`READ)
  );
};

let mergeableState = (data: Js.t('a)) => {
  Belt.Option.(
    data##repository
    ->flatMap(repo => repo##pullRequest)
    ->mapWithDefault(`UNKNOWN, pr => pr##mergeable)
  );
};

let numberOfApprovals = (data: Js.t('a)) => {
  Belt.Option.(
    data##repository
    ->flatMap(repo => repo##pullRequest)
    ->flatMap(pr => pr##approvals)
    ->mapWithDefault(0, approvals => approvals##totalCount)
  );
};

[@react.component]
let make = (~pullRequest) => {
  let mergePullRequest =
    Gql_Schema.MergePullRequest.make(
      ~pullRequestId=pullRequest->pullRequestId,
      (),
    );

  switch (pullRequest->viewerPermission, pullRequest->mergeableState) {
  | (`ADMIN, `MERGEABLE) =>
    <Gql_Mutation.MergePullRequest>
      ...{(mutate, {result}) =>
        switch (result) {
        | NotCalled =>
          <>
            <UI_Control.Button
              text="Merge pull request"
              onClick={_ =>
                mutate(~variables=mergePullRequest##variables, ()) |> ignore
              }
            />
          </>
        | Data(_) => React.null
        | Loading => <div> {"Merging pull request ..." |> React.string} </div>
        | Error(error) => <div> {error##message |> React.string} </div>
        }
      }
    </Gql_Mutation.MergePullRequest>
  | (`WRITE, `MERGEABLE) when pullRequest->numberOfApprovals > 0 =>
    <Gql_Mutation.MergePullRequest>
      ...{(mutate, {result}) =>
        switch (result) {
        | NotCalled =>
          <>
            <UI_Control.Button
              text="Merge pull request"
              onClick={_ =>
                mutate(~variables=mergePullRequest##variables, ()) |> ignore
              }
            />
          </>
        | Data(_) => React.null
        | Loading => <div> {"Merging pull request ..." |> React.string} </div>
        | Error(error) => <div> {error##message |> React.string} </div>
        }
      }
    </Gql_Mutation.MergePullRequest>
  | _ =>
    <span className="notification is-danger">
      "Nah, cannot merge"->React.string
    </span>
  };
};