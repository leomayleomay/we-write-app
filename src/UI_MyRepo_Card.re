[@react.component]
let make = (~repo) => {
  let (editRepoModalIsActive, toggleEditRepoModal) =
    React.useState(() => false);

  <>
    <div className="card" key={repo##id}>
      <header className="card-header">
        <p className="card-header-title"> {repo##name |> React.string} </p>
        <p className="card-header-icon">
          <span className="icon">
            <i className="fas fa-star" ariaHidden=true />
          </span>
          {repo##stargazers##totalCount->string_of_int->React.string}
        </p>
        <p className="card-header-icon">
          <span className="icon">
            <i className="fas fa-code-branch" ariaHidden=true />
          </span>
          {repo##forks##totalCount->string_of_int->React.string}
        </p>
      </header>
      <div className="card-content">
        <div className="content">
          {repo##description
           ->Belt.Option.getWithDefault("No description ~")
           ->React.string}
        </div>
      </div>
      <footer className="card-footer">
        <a
          className="card-footer-item"
          onClick={_ => ReasonReact.Router.push("/" ++ repo##nameWithOwner)}>
          {"View" |> React.string}
        </a>
        <a
          className="card-footer-item"
          onClick={_ => toggleEditRepoModal(_ => true)}>
          {"Edit" |> React.string}
        </a>
        {repo##isPrivate
           ? <div className="card-footer-item">
               <UI_MyRepo_Publish repo />
             </div>
           : React.null}
      </footer>
    </div>
    <UI_Modal
      title="Edit Repo"
      isActive=editRepoModalIsActive
      onClose={() => toggleEditRepoModal(_ => false)}>
      <UI_MyRepos_Form
        repo={Some(repo)}
        onSuccess={() => toggleEditRepoModal(_ => false)}
      />
    </UI_Modal>
  </>;
};