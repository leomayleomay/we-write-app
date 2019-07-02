let baseUrl = "https://we-write-api.herokuapp.com/";

let headers = () =>
  Axios.Headers.fromObj({
    "Authorization": "Bearer " ++ Belt.Option.getExn(Auth.Token.get()),
  });

let getRepositories = () => Axios.get({j|$baseUrl/repositories.json|j});