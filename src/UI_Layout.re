[@react.component]
let make = (~children) => {
  let (newRepoModalIsActive, toggleNewRepoModal) =
    React.useState(() => false);

  Auth.isAuthenticated()
    ? {
      let currentUserQuery = Gql_Schema.CurrentUser.make();
      <Gql_Query.CurrentUser variables=currentUserQuery##variables>
        ...{({result}) =>
          switch (result) {
          | Loading
          | Error(_) => React.null
          | Data(data) =>
            <main>
              <header>
                <nav
                  className="navbar is-primary is-fixed-top" role="navigation">
                  <div className="navbar-brand">
                    <a
                      className="navbar-item"
                      onClick={_ => ReasonReact.Router.push("/")}>
                      <span> {"WeWrite.Works" |> React.string} </span>
                    </a>
                  </div>
                  <div className="navbar-menu">
                    <div className="navbar-start">
                      <a
                        className="navbar-item"
                        onClick={_ => ReasonReact.Router.push("/my/repos")}>
                        <span> {"My Works" |> React.string} </span>
                      </a>
                      <a
                        className="navbar-item"
                        onClick={_ => toggleNewRepoModal(_ => true)}>
                        <span> {"New Works" |> React.string} </span>
                      </a>
                    </div>
                    <div className="navbar-end">
                      <span className="navbar-item">
                        {Js.Json.decodeString(data##viewer##avatarUrl)
                         ->(
                             Belt.Option.mapWithDefault(React.null, avatarUrl =>
                               <img src=avatarUrl alt=data##viewer##login />
                             )
                           )}
                      </span>
                      <span className="navbar-item">
                        {"Logged in as " ++ data##viewer##login |> React.string}
                      </span>
                      <a
                        className="navbar-item"
                        onClick={_ => ReasonReact.Router.push("/auth/logout")}>
                        <span> {"Logout" |> React.string} </span>
                      </a>
                    </div>
                  </div>
                </nav>
              </header>
              <section>
                <Context.CurrentUser value={Some(data##viewer)}>
                  children
                  <UI_Modal
                    title="New Repo"
                    isActive=newRepoModalIsActive
                    onClose={() => toggleNewRepoModal(_ => false)}>
                    <UI_MyRepos_Form
                      repo=None
                      onSuccess={() => toggleNewRepoModal(_ => false)}
                    />
                  </UI_Modal>
                </Context.CurrentUser>
              </section>
            </main>
          }
        }
      </Gql_Query.CurrentUser>;
    }
    : {
      <main>
        <header>
          <nav className="navbar is-primary is-fixed-top" role="navigation">
            <div className="navbar-brand">
              <a
                className="navbar-item"
                onClick={_ => ReasonReact.Router.push("/")}>
                <span> {"WeWrite.Works" |> React.string} </span>
              </a>
            </div>
            <div className="navbar-menu">
              <div className="navbar-end">
                <a
                  className="navbar-item"
                  onClick={_ => ReasonReact.Router.push("/auth/github")}>
                  <span> {"Login/Signup" |> React.string} </span>
                </a>
              </div>
            </div>
          </nav>
        </header>
        <section> children </section>
      </main>;
    };
};