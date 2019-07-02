[@react.component]
let make = (~starrableId, ~viewerHasStarred, ~numberOfStars) => {
  let (starred, setStarred) = React.useState(() => viewerHasStarred);
  let (count, setCount) = React.useState(() => numberOfStars);

  starred
    ? <Gql_Mutation.RemoveStar>
        ...{(mutation, {result}) =>
          switch (result) {
          | NotCalled =>
            <p className="control">
              <a
                className="button"
                onClick={_ => {
                  let removeStar =
                    Gql_Schema.RemoveStar.make(~starrableId, ());
                  mutation(~variables=removeStar##variables, ()) |> ignore;
                  setStarred(_ => false);
                  setCount(count => count - 1);
                }}>
                <span className="icon is-small">
                  <i className="fas fa-star" />
                </span>
                <span> {count->string_of_int->React.string} </span>
              </a>
            </p>
          | Data(_) => React.null
          | Loading => <div> {"Loading" |> React.string} </div>
          | Error(error) => <div> {error##message |> React.string} </div>
          }
        }
      </Gql_Mutation.RemoveStar>
    : <Gql_Mutation.AddStar>
        ...{(mutation, {result}) =>
          switch (result) {
          | NotCalled =>
            <p className="control">
              <a
                className="button"
                onClick={_ => {
                  let addStar = Gql_Schema.AddStar.make(~starrableId, ());
                  mutation(~variables=addStar##variables, ()) |> ignore;
                  setStarred(_ => true);
                  setCount(count => count + 1);
                }}>
                <span className="icon is-small">
                  <i className="far fa-star" />
                </span>
                <span> {count->string_of_int->React.string} </span>
              </a>
            </p>
          | Data(_) => React.null
          | Loading => <div> {"Loading" |> React.string} </div>
          | Error(error) => <div> {error##message |> React.string} </div>
          }
        }
      </Gql_Mutation.AddStar>;
};